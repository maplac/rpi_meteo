from RPLCD import i2c
from influxdb import InfluxDBClient
import time

bme280_valid = False
bme280_t = 0
bme280_p = 0
bme280_h = 0

try:
    client = InfluxDBClient(host='192.168.0.104', port=8086, database='home')
    res = client.query('SELECT mean("temperature"),mean("pressure"),mean("humidity") '
                       'FROM "test" WHERE ("name" = \'bme280\') AND time >= now() - 5m')
    #print(res.raw)
    for point in res.get_points():
        bme280_t = point['mean']
        bme280_p = point['mean_1']
        bme280_h = point['mean_2']
        bme280_valid = True
        break
except Exception as e:
    print("Error: ", e.__class__, ", ", e)


lcd = i2c.CharLCD('PCF8574', 0x27, port=1, charmap='A00', cols=20, rows=4)
lcd.cursor_pos = (0, 0)

t = time.localtime()
time_str = "{:02d}".format(t.tm_hour) + ":" + "{:02d}".format(t.tm_min)
time_str += "  {:02d}".format(t.tm_mday) + ". " + "{:02d}".format(t.tm_mon) + ". " + str(t.tm_year)
lcd.write_string(time_str)
lcd.crlf()

if bme280_valid:
    lcd.write_string("{:.1f}".format(bme280_t) + chr(176) + "C ")
    lcd.write_string("{:.1f}".format(bme280_h) + chr(37) + " ")
    lcd.write_string("{:.0f}".format(bme280_p) + "hPa")
    
else:
    lcd.write_string("no data")

#lcd.crlf()

lcd.close()