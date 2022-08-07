// Board: Arduino Pro Mini
#include<SoftwareSerial.h>
#include <printf.h>

//#define DEBUG_ENABLED 1

#define AVG_TARGET  10
#define PIN_IN      A3
#define PIN_RX       15
#define PIN_TX       16

SoftwareSerial SerialSoft(PIN_RX, PIN_TX);
int sum;
int avgCounter;

void setup() {
  #ifdef DEBUG_ENABLED
    Serial.begin(115200);
    printf_begin();
  #endif
  
  pinMode(PIN_RX, INPUT);
  pinMode(PIN_TX, OUTPUT);
  SerialSoft.begin(9600);
  
  sum = analogRead(PIN_IN);
  avgCounter = 1;
}

void loop() {
  sum += analogRead(PIN_IN);
  avgCounter++;
  if (avgCounter == AVG_TARGET) {
    sum /= (AVG_TARGET * 4);
    if (sum < 0) {
      sum = 0;
    }
    if (sum > 255) {
      sum = 255;
    }
    SerialSoft.write((byte) sum);
    #ifdef DEBUG_ENABLED
      Serial.println((byte) sum);
    #endif
    sum = 0;
    avgCounter = 0;
  }
  delay(100);
}
