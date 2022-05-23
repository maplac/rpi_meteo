// Board: Arduino Pro Mini

#define BUFFER_SIZE 10
#define PIN_OUT     10
#define PIN_IN      A3

int valueBuffer[BUFFER_SIZE];
int writePointer;

int getAverage(int new_value) {
  valueBuffer[writePointer] = new_value;
  writePointer = ++writePointer % BUFFER_SIZE;
  int avg = 0;
  for (int i = 0; i < BUFFER_SIZE; ++i) {
    avg += valueBuffer[i];
  }
  avg /= BUFFER_SIZE;
  return avg;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_OUT, OUTPUT);
  
  int val = analogRead(PIN_IN);
  for (int i = 0; i < BUFFER_SIZE; ++i) {
    valueBuffer[i] = val;
  }
  writePointer = 0;
}

void loop() {
  int val = getAverage(analogRead(PIN_IN));
  //Serial.println(val);
  val /= 4;
  if (val < 10)
    val = 10;
  analogWrite(PIN_OUT, val);
  delay(200);
}
