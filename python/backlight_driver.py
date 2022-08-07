import serial

lastValue = -1
ser = serial.Serial ("/dev/ttyS0", 9600)#timeout=1

while True:
  data = ser.read()
  if len(data) > 0:
    #print(f'received: {data[-1]}')
    value = data[-1] * 2 - 110
    if value > 255:
      value = 255
    if value < 0:
      value = 0
    #print(f'corrected: {value}')
    
    # ignore changes less than 10
    if abs(value - lastValue) < 10:
      continue
      
    lastValue = value
    print(f'backlight value: {value}')
    
    try:
      with open("/sys/class/backlight/rpi_backlight/brightness", "w") as f:
        f.write(str(data[-1]))
    except Exception as e:
      print("Error: ", e.__class__, ", ", e)
 
