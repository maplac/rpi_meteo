import pigpio
import RPi.GPIO as GPIO
import time

INPUT_PIN = 23
hostname = 'localhost'
port = 8888

def gpio_interrupt(gpio):
  print("int")

# Connect to pigpiod
print(f'Connecting to GPIO daemon on {hostname}:{port} ...')
pi = pigpio.pi(hostname, port)
if not pi.connected:
    print("Not connected to Raspberry Pi!")
    sys.exit()

#pi.callback(INPUT_PIN, pigpio.FALLING_EDGE, gpio_interrupt)
#while True:
#  time.sleep(1)

GPIO.setmode(GPIO.BCM)
GPIO.setup(INPUT_PIN, GPIO.IN)
GPIO.add_event_detect(INPUT_PIN, GPIO.FALLING, callback=gpio_interrupt, bouncetime=200) # Wait for the input to go low, run the function when it does
while True:
  time.sleep(1)

#while True:
#  if (GPIO.input(INPUT_PIN) == True):
#    print('1')
#  else:
#    print('0')
#  time.sleep(1);

 