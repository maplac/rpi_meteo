import serial
from time import sleep


dataBuffer = []
syncChar = 0b10101010
sizeIndex = 4
checkSumIndex = 5

def processBuffer(dataBuffer):
  res = 0
  for i in range(0, len(dataBuffer) - 6):
    if dataBuffer[i] == syncChar and dataBuffer[i + 1] == syncChar and dataBuffer[i + 2] == syncChar and dataBuffer[i + 3] == syncChar:
      size = dataBuffer[i + sizeIndex]
      checkSumRec = dataBuffer[i + checkSumIndex]
      if len(dataBuffer) >= (size + 4):
        print(f'{i = }, {size = }, {checkSumRec = }')
        print("data: ", end='')
        for j in range(0, size):
          print(str(dataBuffer[i + j]) + ", ", end='')
        print("")
        #todo checksum, process payload
        del dataBuffer[i:(i + 4 + size)]
        res = 1
        break
  return res

ser = serial.Serial ("/dev/ttyS0", 9600, timeout=1)    #Open port with baud rate
while True:
  newData = ser.read()              #read serial port
  if len(newData) == 0:
    if len(dataBuffer) > 0:
      print("Incomplete data buffer cleared.")
      print(str(dataBuffer)[1:-1]) 
      dataBuffer = []
    continue
  
  dataBuffer += newData
  
  if len(dataBuffer) >= 6:
    #print("dataBuffer len: " + str(len(dataBuffer)))
    processed = processBuffer(dataBuffer)
    while processed:
      processed = processBuffer(dataBuffer)
    
 