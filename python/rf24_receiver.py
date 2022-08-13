from datetime import datetime
import struct
import sys
import time
import traceback

from influxdb import InfluxDBClient
import pigpio
import RPi.GPIO as GPIO
from nrf24 import *

#def gpio_interrupt(gpio, level, tick):
def gpio_interrupt(gpio):
    global count
    
    # Interrupt cause by data being available.
    #print(f'Interrupt: gpio={gpio}, level={("LOW", "HIGH", "NONE")[level]}, tick={tick}')

    # As long as data is ready for processing, process it.
    while nrf.data_ready():
      try:
        # Count message and record time of reception.            
        
        # Read pipe and payload for message.
        pipe = nrf.data_pipe()
        payload = nrf.get_payload()
        
        # Show message received as hex.
        #now = datetime.now()
        #hex = ':'.join(f'{i:02x}' for i in payload)
        #print(f"{now:%Y-%m-%d %H:%M:%S.%f}: pipe: {pipe}, len: {len(payload)}, bytes: {hex}, count: {count}")

        msg = [];

        values = struct.unpack("<BBxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", payload)
        device_id = values[0]
        if device_id == 1:
          values = struct.unpack("<BBxxffffLxxxxxxxx", payload)
          print(f'ID: {values[0]}, counter: {values[1]}, temperature: {values[2]}, temperature2: {values[3]} humidity: {values[4]}, voltage: {values[5]}, lost: {values[6]}')
          msg = [{
            "measurement": "rpi_meteo",
            "tags": {
              "name": "dht22"
            },
            "fields": {
              "temperature": values[2],
              "temperature2": values[3],
              "humidity": values[4],
              "voltage": values[5],
              "lost": values[6]
            }
          }]
        
        if len(msg) > 0:
          client = InfluxDBClient(host='127.0.0.1', port=8086, database='home')
          client.write_points(msg)
          
      except Exception as e:
        print("Error: ", e.__class__, ", ", e)


if __name__ == "__main__":

    hostname = 'localhost'
    port = 8888
    address = 'RpiMe'# length must between 3 and 5 characters

    # Connect to pigpiod
    print(f'Connecting to GPIO daemon on {hostname}:{port} ...')
    pi = pigpio.pi(hostname, port)
    if not pi.connected:
        print("Not connected to Raspberry Pi!")
        sys.exit()

    # Setup callback for interrupt triggered when data is received.
    #pi.callback(24, pigpio.FALLING_EDGE, gpio_interrupt)
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(24, GPIO.IN)
    GPIO.add_event_detect(24, GPIO.FALLING, callback=gpio_interrupt)

    # Create NRF24 object.
    # PLEASE NOTE: PA level is set to MIN because test sender/receivers are often close to each other, and then MIN works better.
    nrf = NRF24(pi, ce=25, payload_size=RF24_PAYLOAD.DYNAMIC, channel=100, data_rate=RF24_DATA_RATE.RATE_250KBPS, pa_level=RF24_PA.MAX)
    nrf.set_address_bytes(len(address))

    # Listen on the address specified as parameter
    nrf.open_reading_pipe(RF24_RX_ADDR.P1, address)
    
    # Display the content of NRF24L01 device registers.
    nrf.show_registers()

    try:
        while True:
            time.sleep(1)
    except:
        traceback.print_exc()
        nrf.power_down()
        pi.stop()
