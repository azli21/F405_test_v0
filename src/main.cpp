#include <Arduino.h>
#include <BlinkLED.h>

BlinkLED blinker(PB5, 1); // LED on PB5, blinking at 1 Hz

#define BOOTLOADER_MAGIC 0xDEADBEEF

// .noinit: not zeroed at startup — value survives NVIC_SystemReset()
__attribute__((section(".noinit"))) volatile uint32_t _bootFlag;

void jumpToBootloader() {
  SerialUSB.println("Resetting to DFU bootloader...");
  delay(100);
  _bootFlag = BOOTLOADER_MAGIC;
  NVIC_SystemReset();
}

void setup() {
  // Check magic BEFORE USB initialises
  // Only jump to bootloader if this was a SOFTWARE reset (not power-on/watchdog)
  bool isSoftReset = (RCC->CSR & RCC_CSR_SFTRSTF);
  __HAL_RCC_CLEAR_RESET_FLAGS();

  if (isSoftReset && _bootFlag == BOOTLOADER_MAGIC) {
    _bootFlag = 0;

    // Reset and disable USB OTG FS so the ROM bootloader gets a clean state
    RCC->AHB2RSTR |=  RCC_AHB2RSTR_OTGFSRST;
    __NOP(); __NOP();
    RCC->AHB2RSTR &= ~RCC_AHB2RSTR_OTGFSRST;
    RCC->AHB2ENR  &= ~RCC_AHB2ENR_OTGFSEN;

    // Disable SysTick and all interrupts
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    for (int i = 0; i < 8; i++) {
      NVIC->ICER[i] = 0xFFFFFFFF;
      NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    __disable_irq();
    HAL_RCC_DeInit(); // reset clocks to HSI — required by ROM bootloader

    const uint32_t addr = 0x1FFF0000; // STM32F405 ROM bootloader
    __set_MSP(*(uint32_t*)addr);
    ((void (*)(void))(*(uint32_t*)(addr + 4)))();
  }

  SerialUSB.begin(); // USB CDC serial
  // pinMode handled in BlinkLED constructor
}

void loop() {
  // Type 'b' in the serial monitor to enter DFU mode for USB upload
  if (SerialUSB.available() && SerialUSB.read() == 'b') {
    jumpToBootloader();
  }

  if (blinker.run()) {
    SerialUSB.print("LED: ");
    SerialUSB.println(digitalRead(PB5) ? "ON" : "OFF");
  }
}




