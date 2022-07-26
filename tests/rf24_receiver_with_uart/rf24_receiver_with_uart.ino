
#include <JeeLib.h>
#include <printf.h>
#include <RF24.h>
#include<SoftwareSerial.h>

#define DEBUG_ENABLED 1

#define rxPin 7
#define txPin 8
SoftwareSerial SerialSoft(rxPin, txPin);

// Cretate NRF24L01 radio.
#define DATA_LEN  32
RF24 radio(14, 10);// CE pin, CSN pin
byte rf24_tx[6] = "RpiMe";    // Address used when transmitting data.
byte payload[DATA_LEN];             // Payload bytes. Used both for transmitting and receiving

byte syncChar = 0b10101010;

void setup() {

  #ifdef DEBUG_ENABLED
    Serial.begin(115200);
    printf_begin();
  #endif

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  SerialSoft.begin(9600);

  // Configure the NRF24 tranceiver.
  Serial.println("Configure NRF24 ...");
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(5, 15);//Delay bewteen retries, 1..15.  Multiples of 250µs. Number of retries, 1..15.
  radio.setDataRate(RF24_250KBPS);// RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  radio.setChannel(100);// 0 ... 125
  radio.setCRCLength(RF24_CRC_16);// RF24_CRC_DISABLED, RF24_CRC_8, RF24_CRC_16 for 16-bit
  radio.setPayloadSize(32);// Max. 32 bytes.
  radio.openReadingPipe(0, rf24_tx);
  radio.startListening();

  #ifdef DEBUG_ENABLED
    // Show debug information for NRF24 tranceiver.
    radio.printDetails();
  #endif
}

void loop() {
  if (radio.available()) {
    radio.read(&payload, DATA_LEN);    //Reading the data
    int sum = DATA_LEN + 2;
    for(int i = 0; i < DATA_LEN; i++){
      sum += payload[i];
    }
    byte checkSum = sum & 0xFF;
    for(int i = 0; i < 4; i++){
      SerialSoft.write(syncChar);
    }
    SerialSoft.write(DATA_LEN + 2);
    SerialSoft.write(checkSum);
    SerialSoft.write(payload, DATA_LEN);
    
    #ifdef DEBUG_ENABLED
      Serial.print("payload: ");
      for(int i = 0; i < DATA_LEN; i++){
        if (i > 0)
          Serial.print(", ");
        Serial.print(payload[i]);
      }
      Serial.println("");
    #endif
  }
}
