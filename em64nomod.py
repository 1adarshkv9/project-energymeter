import serial
import struct
import time

# Attempt to open the serial port
try:
    ser = serial.Serial('/dev/ttyUSB0', baudrate=9600, bytesize=8, parity='E', stopbits=1)
except serial.SerialException:
    print("Error: Serial port not available or could not be opened.")
    exit(1)  # Exit the script with an error code

# Function to send a Modbus request and read the response
def read_register(slave_address, register_address, num_registers):
    #  request frame
    request = bytearray([
        slave_address,  # Slave address
        0x03,           # Function code for reading analog output holding registers
        (register_address >> 8) & 0xFF,  # High byte of register address
        register_address & 0xFF,         # Low byte of register address
        (num_registers >> 8) & 0xFF,     # High byte of number of registers
        num_registers & 0xFF              # Low byte of number of registers
    ])
    
    # Calculate CRC16 checksum
    crc = 0xFFFF
    for byte in request:
        crc ^= byte
        for _ in range(8):
            if crc & 0x01:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    request.append(crc & 0xFF)
    request.append((crc >> 8) & 0xFF)
    
    # Send the request
    ser.write(request)
    
    # Wait for the response
    time.sleep(0.1)  # Adjust this delay based on your device's response time
    
    # Read the response
    response = ser.read(5 + 2 * num_registers)  # 5 bytes for the header and 2 bytes per register
    
    # Check if the response is valid
    if len(response) != 5 + 2 * num_registers:
        #print("Invalid response length")
        return None
    
    # Extract the data bytes and CRC
    data = response[:-2]
    received_crc = struct.unpack('>H', response[-2:])[0]
    print(received_crc)
    
    # Calculate CRC16 checksum for received data
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x01:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    
    # Check CRC
    if crc != received_crc:
        print("CRC check failed")
        return None
    
    # Extract the 32-bit float value from the data
    value = struct.unpack('>f', data[3:] + data[1:3])[0]
    
    return value

# Main loop
counter = 1
while True:
    print("==========================================================")
    print("Iteration {}".format(counter))
    print("----------------------------------------------------------")
    
    # Read the Watt-hour register (address 3900) from unit 1
    watt_hour_reading = read_register(slave_address=1, register_address=3900, num_registers=2)
    
    if watt_hour_reading is not None:
        print("Watt Hour Reading:", watt_hour_reading)
    
    counter += 1
    
    print("==========================================================")
    
    time.sleep(2)
