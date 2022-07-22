
#include <DHT.h>
#include<Vcc.h>
#include <JeeLib.h>
#include <printf.h>
#include <RF24.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

//#define DEBUG_ENABLED 1
#define USE_DHT22     1
#define USE_DS18      1

#define DEVICE_ID   1
#define PIN_VOLTAGE A4
#define PIN_BUTTON  2
#define PIN_LED     17

// Cretate NRF24L01 radio.
RF24 radio(14, 10);// CE pin, CSN pin
byte rf24_tx[6] = "RpiMe";    // Address used when transmitting data.
byte payload[32];             // Payload bytes. Used both for transmitting and receiving

#ifdef USE_DHT22
  // Create DHT sensor
  DHT dht(19, DHT22);// pin number, DHT type (DHT11, DHT12, DHT21, DHT22 (AM2302), AM2301)
#endif

#ifdef USE_DS18
  // Create DS18B20 sensor
  OneWire oneWire(16);// pin number
  DallasTemperature ds18(&oneWire);
#endif

// flags
volatile bool isTimeout = false;
volatile bool isButton = false;

uint32_t counterSendFailed = 0;
uint8_t counterPackets = 0;
uint8_t buttonLastState = 3;
uint32_t counterSendAttempts = 0;

void measureAndSend(void);

ISR(WDT_vect) {
  Sleepy::watchdogEvent();
  isTimeout = true;
}

void buttonInt(void){
  isButton = true;
}

void setup() {
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonInt, FALLING);
  analogReference(DEFAULT);//EXTERNAL

  #ifdef DEBUG_ENABLED
    Serial.begin(115200);
    printf_begin();
  #endif

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
  radio.openWritingPipe(rf24_tx);
  radio.stopListening();

  #ifdef DEBUG_ENABLED
    // Show debug information for NRF24 tranceiver.
    radio.printDetails();
  #endif

  #ifdef USE_DHT22
    // Initialise the DHT sensor.
    dht.begin();
  #endif

  isTimeout = true;
}

void loop() {
  
  // make sure everything is finished before go to sleep
  delay(20);

  // wait for button press or timer
  if (!isButton && !isTimeout){
    Sleepy::loseSomeTime(60000);
  }

  if (isButton) {
    delay(100);// debounce
    if (!digitalRead(PIN_BUTTON)) {
      measureAndSend();
    }
    isButton = false;
  }
  if (isTimeout) {
    isTimeout = false;
    measureAndSend();
  }
}

void measureAndSend(void) {
   
  float t, h, v, t2;
  v = analogRead(PIN_VOLTAGE) * 3.3 / 1024.0;
  #ifdef USE_DS18
    ds18.requestTemperatures();
  #endif
  #ifdef USE_DHT22
    h = dht.readHumidity();
    t = dht.readTemperature();
  #else
    h = 0.0;
    t = 0.0;
  #endif
  #ifdef USE_DS18
    t2 = ds18.getTempCByIndex(0);
  #else
    t2 = 0.0;
  #endif

  #ifdef DEBUG_ENABLED
    // Report the temperature and humidity.    
    Serial.print("temperature = "); Serial.print(t); 
    Serial.print(", temperature2 = "); Serial.print(t2);
    Serial.print(", humidity = "); Serial.print(h);
    Serial.print(", voltage = "); Serial.print(v);
    Serial.print(", lostPackets = "); Serial.println(counterSendFailed);
  #endif

  digitalWrite(PIN_LED, HIGH);

  payload[0] = DEVICE_ID; // id
  payload[1] = counterPackets; // packet counter
  payload[2] = 0; // packet type
  payload[3] = 0; // reserved

  float *packetF = (float*) &payload; //size_of(float)=4
  packetF[1] = t;
  packetF[2] = t2;
  packetF[3] = h;
  packetF[4] = v;

  uint32_t *packetUI23 = (uint32_t*) &payload;
  packetUI23[5] = counterSendFailed;

  if (radio.write(payload, 32)) {
    counterSendFailed = 0;
    #ifdef DEBUG_ENABLED
      Serial.print("Transfer OK. Retries="); Serial.println(radio.getARC());
    #endif
  } else {
    counterSendFailed++;
    #ifdef DEBUG_ENABLED
      Serial.print("Transfer FAILED. Retries="); Serial.println(radio.getARC());
    #endif
  }
  counterPackets++;

  radio.startListening();
  radio.stopListening();
  digitalWrite(PIN_LED, LOW);
}
