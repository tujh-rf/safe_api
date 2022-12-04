Task

1. Open communication with two devices with ID '1' and '2'
2. First device MUST be power up by writing a special command
3. Second device is already powered up
4. Read from the first device signal 'Ready'
5. Send 'Hello' command to both devices

List of commands
1. POWER_ON     address 0x00:0x00, 8 bit, value to write is 0xFD
2. READY        address 0xAA:0xFF, 8 bit, value when ready is 0x2A
3. HELLO        address 0x10:0xA0, 16 bits, value to write is 0x100, value to read 0x2A
