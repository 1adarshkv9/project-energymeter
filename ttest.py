from pymodbus.client.sync import ModbusSerialClient as ModbusClient
from pymodbus.payload import BinaryPayloadDecoder
from pymodbus.constants import Endian
import time
client = ModbusClient(method='rtu', port='/dev/ttymxc5', stopbits=1, bytesize=8, baudrate=9600, parity='E')

connection = client.connect()

if not connection:
    print("Modbus connection failed.")
    exit(1)

counter = 1

while True:
    try:
        print("==========================================================")
        print("Iteration {}".format(counter))
        print("----------------------------------------------------------")

        # Read the Watt-hour register (0x00) from unit 1
        WattHour = client.read_holding_registers(3900, 2, unit=1)

        if WattHour.isError():
            print("Modbus read error: {}".format(WattHour))
        else:
            decoder = BinaryPayloadDecoder.fromRegisters(WattHour.registers, Endian.Big, wordorder=Endian.Little)
            watt_hour_reading = decoder.decode_32bit_float()
            print("Watt Hour Reading:", watt_hour_reading)

        counter += 1

    except Exception as e:
        print("An error occurred:", str(e))

    print("==========================================================")

    time.sleep(2)

