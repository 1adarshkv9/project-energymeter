#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// Define the serial port device
#define SERIAL_PORT "/dev/ttyUSB0"

// Function to set up the serial port
int setup_serial_port() {
    int fd;
    struct termios serial_settings;

    // Open the serial port in non-blocking read/write mode
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Failed to open serial port");
        return -1;
    }

    // Configure the serial port settings (9600 baud, 8 data bits, no parity, 1 stop bit)
    memset(&serial_settings, 0, sizeof(serial_settings));
    serial_settings.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    serial_settings.c_iflag = IGNPAR;
    serial_settings.c_oflag = 0;
    serial_settings.c_lflag = 0;
    serial_settings.c_cc[VTIME] = 1;  // Timeout in tenths of a second
    serial_settings.c_cc[VMIN] = 4;   // Read at least 4 characters

    if (tcsetattr(fd, TCSANOW, &serial_settings) != 0) {
        perror("Failed to configure serial port");
        close(fd);
        return -1;
    }

    return fd;
}

// Function to send a command and receive a response
int send_command(int fd, const char *command, char *response) {
    // Send the command (register address) to the serial port
    ssize_t bytes_written = write(fd, command, strlen(command));
    if (bytes_written == -1) {
        perror("Failed to write to serial port");
        return -1;
    }

    // Read the response from the serial port
    ssize_t bytes_read = read(fd, response, 4); // Assuming a 4-byte response, adjust as needed
    if (bytes_read == -1) {
        perror("Failed to read from serial port");
        return -1;
    }

    return bytes_read;
}

// Function to combine two 16-bit registers into a 32-bit integer
uint32_t combine_registers(const char *response) {
    uint16_t value1, value2;
    memcpy(&value1, response, 2);
    memcpy(&value2, response + 2, 2);
    return ((uint32_t)value1 << 16) | value2;
}

int main() {
    int serial_fd;
    char response[4];

    // Set up the serial port
    serial_fd = setup_serial_port();
    if (serial_fd == -1) {
        return -1;
    }

    while (1) {
        // Send a command to request data from a specific register (adjust as needed)
        const char *command = "READ_REGISTER 0x0046";

        // Send the command and receive the response
        int bytes_read = send_command(serial_fd, command, response);
        if (bytes_read == -1) {
            close(serial_fd);
            return -1;
        }

        // Combine the two 16-bit registers into a 32-bit integer
        uint32_t combined_value = combine_registers(response);

        // Print the combined value
        printf("Combined Value: %u\n", combined_value);

        // Sleep for a while before sending the next command
        sleep(1);
    }

    // Close the serial port
    close(serial_fd);

    return 0;
}

