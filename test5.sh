#!/bin/bash

# Define the serial port device
SERIAL_PORT="/dev/ttyUSB0"


# Configure the serial port (baud rate, data bits, parity, stop bits)
stty -F $SERIAL_PORT 9600 cs8 -parenb -cstopb

# Function to send a command and receive a response
send_command() {
    # Send the command (register address) to the serial port
    echo -n -e "$1" > $SERIAL_PORT

    cat $SERIAL_PORT > /dev/null 2>&1
    

    # Read and print the response from the serial port (adjust the length as needed)
    read -n 16 -t 2 response < $SERIAL_PORT
    echo "Response: $response"
}

# Loop to continuously send commands and receive responses
while true; do
    # Send a command to request data from a specific register (adjust as needed)
    send_command "READ_REGISTER 0x00"

    # Sleep for a while before sending the next command
    sleep 1
done

