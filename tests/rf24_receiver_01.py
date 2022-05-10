import argparse
from datetime import datetime
import struct
import sys
import time
import traceback

import pigpio
from nrf24 import *


#
# A simple NRF24L receiver that connects to a PIGPIO instance on a hostname and port, default "localhost" and 8888, and
# starts receiving data on the address specified.  Use the companion program "int-sender.py" or "simple-sender.py" to 
# send data to it from a different Raspberry Pi.
#

# Count of messages received.
count = 0

def gpio_interrupt(gpio, level, tick):
    global count

    # Interrupt cause by data being available.
    print(f'Interrupt: gpio={gpio}, level={("LOW", "HIGH", "NONE")[level]}, tick={tick}')

    # As long as data is ready for processing, process it.
    while nrf.data_ready():
        # Count message and record time of reception.            
        count += 1
        now = datetime.now()
        
        # Read pipe and payload for message.
        pipe = nrf.data_pipe()
        payload = nrf.get_payload()
        hex = ':'.join(f'{i:02x}' for i in payload)

        # Show message received as hex.
        print(f"{now:%Y-%m-%d %H:%M:%S.%f}: pipe: {pipe}, len: {len(payload)}, bytes: {hex}, count: {count}")

        values = struct.unpack("<BBxxffffxxxxxxxxxxxx", payload)
        print(f'ID: {values[0]}, counter: {values[1]}, temperature: {values[2]}, pressure: {values[3]}, humidity: {values[4]}, voltage: {values[5]}')


if __name__ == "__main__":

    print("Python NRF24 Simple Interrupt Based Receiver Example.")
    
    hostname = 'localhost'
    port = 8888
    address = 'RpiMe'# length must between 3 and 5 characters

    # Connect to pigpiod
    print(f'Connecting to GPIO daemon on {hostname}:{port} ...')
    pi = pigpio.pi(hostname, port)
    if not pi.connected:
        print("Not connected to Raspberry Pi ... goodbye.")
        sys.exit()

    # Setup callback for interrupt triggered when data is received.
    pi.callback(24, pigpio.FALLING_EDGE, gpio_interrupt)

    # Create NRF24 object.
    # PLEASE NOTE: PA level is set to MIN because test sender/receivers are often close to each other, and then MIN works better.
    nrf = NRF24(pi, ce=25, payload_size=RF24_PAYLOAD.DYNAMIC, channel=100, data_rate=RF24_DATA_RATE.RATE_250KBPS, pa_level=RF24_PA.MIN)
    nrf.set_address_bytes(len(address))

    # Listen on the address specified as parameter
    nrf.open_reading_pipe(RF24_RX_ADDR.P1, address)
    
    # Display the content of NRF24L01 device registers.
    nrf.show_registers()

    # Enter a loop to prevent us from stopping.
    try:
        print(f'Receive from {address}')        
        while True:
            # Sleep 1 second.
            time.sleep(1)
    except:
        traceback.print_exc()
        nrf.power_down()
        pi.stop()
