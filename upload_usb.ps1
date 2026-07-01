# USB DFU Upload Script for STM32F405
# Sends 'b' to COM3 and keeps port open until the board resets into DFU mode,
# then uploads via STM32CubeProgrammer

$port     = "COM3"
$firmware = "$PSScriptRoot\.pio\build\genericSTM32F405RG\firmware.bin"
$cubeprog = "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"

Write-Host "Opening $port..."
$serial = New-Object System.IO.Ports.SerialPort $port, 115200
$serial.DtrEnable = $true
$serial.Open()
Start-Sleep -Milliseconds 1000   # Let USB CDC stabilise

Write-Host "Sending bootloader command 'b'..."
$serial.Write("b")

# Keep port open and wait for board to disconnect (reset into DFU)
Write-Host "Waiting for board to reset into DFU mode..."
$start = Get-Date
$disconnected = $false
while (-not $disconnected) {
    try {
        $_ = $serial.BytesToRead   # throws when USB disconnects
        Start-Sleep -Milliseconds 100
        if ((Get-Date) - $start -gt [TimeSpan]::FromSeconds(10)) {
            Write-Host "ERROR: Board did not reset within 10 seconds."
            $serial.Close()
            exit 1
        }
    } catch {
        $disconnected = $true
    }
}

Write-Host "Board reset detected. Waiting for DFU device to enumerate..."
Start-Sleep -Seconds 3

Write-Host "Uploading firmware..."
& $cubeprog -c port=USB1 -d $firmware 0x08000000 -rst
exit $LASTEXITCODE
