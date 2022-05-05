// DHT sensor library, version 1.4.0 by Adafruit
#include <DHT.h>

// RF24, version 1.3.9, by TMRh20
#include <printf.h>
#include <RF24.h>

#define DEVICE_ID 1

#define PIN_DHT                  8            // PIN for DHT sensor communication.
#define DHT_TYPE              DHT22           // Type of DHT sensor:
                                              // DHT11, DHT12, DHT21, DHT22 (AM2302), AM2301
                                      
// Cretate NRF24L01 radio.
RF24 radio(10, 9);// CE pin, CSN pin

// Create DHT sensor
DHT dht(PIN_DHT, DHT_TYPE);

byte rf24_tx[6] = "RpiMe";    // Address used when transmitting data.
byte payload[32];             // Payload bytes. Used both for transmitting and receiving

unsigned long last_reading;                // Milliseconds since last measurement was read.
unsigned long ms_between_reads = 10000;    // 10000 ms = 10 seconds
uint32_t counterSendFailed = 0;
uint8_t counterPackets = 0;

void setup() {
  
  // Initialize serial connection.
  Serial.begin(115200);
  printf_begin();
  delay(100);
  
  // Show that program is starting.
  Serial.println("\n\nNRF24L01 Arduino Simple Sender.");

  // Configure the NRF24 tranceiver.
  Serial.println("Configure NRF24 ...");
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.setPALevel(RF24_PA_MIN);
  radio.setRetries(5, 15);//Delay bewteen retries, 1..15.  Multiples of 250µs. Number of retries, 1..15.
  radio.setDataRate(RF24_250KBPS);// RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  radio.setChannel(100);// 0 ... 125
  radio.setCRCLength(RF24_CRC_16);// RF24_CRC_DISABLED, RF24_CRC_8, RF24_CRC_16 for 16-bit
  radio.setPayloadSize(32);// Max. 32 bytes.
  radio.openWritingPipe(rf24_tx);
  radio.stopListening();
  
  // Show debug information for NRF24 tranceiver.
  radio.printDetails();
  
  // Initialise the DHT sensor.
  dht.begin();

  // Take the current timestamp. This means that the next (first) measurement will be read and
  // transmitted in "ms_between_reads" milliseconds.
  last_reading = 0;
}

void loop() {

  if (millis() - last_reading > ms_between_reads) {
    // Read sensor values every "ms_between_read" milliseconds.
  
    // Read the humidity and temperature.
    float t, h, p, v;
    h = dht.readHumidity();
    t = dht.readTemperature();
    
    // Report the temperature and humidity.    
    Serial.print("Sensor values: temperature="); Serial.print(t); 
    Serial.print(", humidity="); Serial.println(h);

    // Stop listening on the radio (we can't both listen and send).
    radio.stopListening();

    // Send the data ...
    int offset = 0;  
    //Serial.println("Preparing payload.");
    payload[0] = DEVICE_ID; // id
    payload[1] = counterPackets; // packet counter
    payload[2] = 0; // packet type
    payload[3] = 0; // reserved
  
    float *packetF = (float*) &payload; //size_of(float)=4
    packetF[1] = t;
    packetF[2] = p;
    packetF[3] = h;
    packetF[4] = v;
  
    uint32_t *packetUI23 = (uint32_t*) &payload;
    packetUI23[5] = counterSendFailed;
  
    if (radio.write(payload, 32)) {
      Serial.print("Payload sent successfully. Retries="); Serial.println(radio.getARC());
    } else {
      counterSendFailed++;
      Serial.print("Failed to send payload. Retries="); Serial.println(radio.getARC());
    }
    counterPackets++;

    // Start listening again.
    radio.startListening();

    // Register that we have read the temperature and humidity.
    last_reading = millis();
  }
}
