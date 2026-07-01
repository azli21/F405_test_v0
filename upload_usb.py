"""
USB DFU Upload Script for STM32F405
Sends 'b' to the board via USB CDC, waits for it to enter DFU bootloader,
then uploads firmware via STM32CubeProgrammer CLI.
"""
import serial
import time
import subprocess
import sys
import os

PORT      = "COM3"
FIRMWARE  = os.path.join(os.path.dirname(__file__), r".pio\build\genericSTM32F405RG\firmware.bin")
CUBEPROG  = r"C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"

print(f"Opening {PORT}...")
try:
    s = serial.Serial(PORT, 115200, timeout=1)
except serial.SerialException as e:
    print(f"ERROR: Could not open {PORT}: {e}")
    sys.exit(1)

time.sleep(1.5)  # Let USB CDC stabilise

print("Sending bootloader command 'b'...")
s.write(b'b')
s.flush()

print("Waiting for board to reset into DFU mode...")
start = time.time()
disconnected = False
while time.time() - start < 10:
    try:
        s.read(1)  # raises SerialException when USB disconnects
        time.sleep(0.05)
    except serial.SerialException:
        disconnected = True
        print("Board disconnected — entering DFU mode!")
        break

try:
    s.close()
except Exception:
    pass

if not disconnected:
    print("ERROR: Board did not reset within 10 seconds.")
    sys.exit(1)

print("Waiting for DFU device to enumerate on USB...")
time.sleep(3)

print("Uploading firmware via STM32CubeProgrammer...")
result = subprocess.run([CUBEPROG, "-c", "port=USB1", "-d", FIRMWARE, "0x08000000", "-rst"])
sys.exit(result.returncode)
