import board
from adafruit_bme280 import basic as adafruit_bme280
from influxdb import InfluxDBClient

try:
    # Create sensor object, using the board's default I2C bus.
    i2c = board.I2C()   # uses board.SCL and board.SDA
    bme280 = adafruit_bme280.Adafruit_BME280_I2C(i2c)
    t = bme280.temperature
    h = bme280.relative_humidity
    p = bme280.pressure

    #print("\nTemperature: %0.1f C" % t)
    #print("Humidity: %0.1f %%" % h)
    #print("Pressure: %0.1f hPa" % p)
    msg = [{
        "measurement": "rpi_meteo",
        "tags": {
            "name": "bme280"
        },
        "fields": {
            "temperature": t,
            "humidity": h,
            "pressure": p
        }
    }]
    client = InfluxDBClient(host='127.0.0.1', port=8086, database='home')
    client.write_points(msg)
except Exception as e:
    print("Error: ", e.__class__, ", ", e)

