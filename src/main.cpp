#include <Arduino.h>
#include <BlinkLED.h>

BlinkLED blinker(PB5, 3); // LED on PB5, blinking at 3 Hz

void setup() {
  // pinMode handled in BlinkLED constructor
}

void loop() {
  blinker.run();
}

