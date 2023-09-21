#include <stdio.h>
#include <stdlib.h>
#include <modbus/modbus.h>
#include <inttypes.h>
#include <unistd.h>

float read_float_registers(modbus_t *ctx, int address) {
    uint16_t registers[2];
    float value = 0.0f;

    if (modbus_read_registers(ctx, address, 2, registers) != -1) {
        uint32_t combined_value = ((uint32_t)registers[0] << 16) | registers[1];
        value = *((float *)&combined_value);
    }

    return value;
}

int main() {
    modbus_t *ctx = NULL;
    ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);

    if (ctx == NULL) {
        fprintf(stderr, "Failed to create the Modbus context\n");
        return -1;
    }

    modbus_set_slave(ctx, 1);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Modbus connection failed: %s\n", modbus_strerror(perror));
        modbus_free(ctx);
        return -1;
    }

    uint32_t counter = 1;

    while (1) {
        printf("==========================================================\n");
        printf("Iteration %" PRIu32 "\n", counter);
        printf("----------------------------------------------------------\n");

        // Read Voltage (32-bit floating point)
        float voltage_value = read_float_registers(ctx, 0x00);
        printf("Voltage: %f\n", voltage_value);

        // Read Current (32-bit floating point)
        float current_value = read_float_registers(ctx, 0x0102);
        printf("Current: %f\n", current_value);

        // Read Watt (32-bit floating point)
        float watt_value = read_float_registers(ctx, 0x009E);
        printf("Watt: %f\n", watt_value);

        // Read Frequency (32-bit floating point)
        float freq_value = read_float_registers(ctx, 0x0046);
        printf("Frequency: %f\n", freq_value);

        sleep(2);
        counter++;
    }

    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}

