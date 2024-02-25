import serial
from rpi_backlight import Backlight

lastValue = -1
ser = serial.Serial ("/dev/ttyS0", 9600)#timeout=1
backlight = Backlight()

while True:
  data = ser.read()
  if len(data) > 0:
    #print(f'received: {data[-1]}')
    value = (data[-1] * 2 - 110) / 2.55
    if value > 100:
      value = 100
    if value < 0:
      value = 0
    #print(f'corrected: {value}')
    
    if value == lastValue:
      continue
      
    lastValue = value
    print(f'backlight value: {value}')
    
    backlight.brightness = value
    
#    try:
#      with open("/sys/class/backlight/rpi_backlight/brightness", "w") as f:
#        f.write(str(data[-1]))
#    except Exception as e:
#      print("Error: ", e.__class__, ", ", e)

