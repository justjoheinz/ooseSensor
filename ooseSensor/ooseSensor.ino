#include <SensuinoEth.h>
#include <SensuinoSerial.h>
#include <Ethernet.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

SensuinoEth pf;

const long int device_id = 15200; // This is your device's ID

const unsigned long int feed_41206 = 41206; // This is the feed "temperature-sensor"

const unsigned long int feed_41207 = 41207; // This is the feed "light-sensor"

unsigned long timeRef;

int val = 0;
float tempVal = 0;
char buffer[30];

#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// light resistence sensor, analog
const int lightPin = A0;

void setup() {
  Serial.begin(9600);  
  byte err;
  // Use the following if you don't want to use DHCP
  // pf.init(&err, 0, "aa:bb:cc:dd:ee:ff", "192.168.1.150", "192.168.1.1");// First IP is for the Arduino, second IP is for the gateway
  pf.init(&err, 1, "aa:bb:cc:dd:ee:ff");
  delay(1000);
  
  pf.setSenseKey("pi_eN3hsIlLYcBhcmweJlg");// This is your sense key

  /* We set the PINs */
  
  pinMode(5, INPUT);
  pinMode(0, INPUT);
  Serial.println("Init done");

  // For each output feed, get the last value and trigger the right PIN on the arduino
  
  //We listen for incoming events on the device
  pf.longPollSense(device_id);
  
  // Init timeRef
  timeRef = millis();
}

void loop() {
  byte _check = pf.longPollCheck();
    if(_check == 0) {
      pf.longPollSense(device_id);
      
      unsigned long int feed_id = pf.getLastFeedId();
      char *str = pf.getLastValue();
          
    } else if(_check == 2) {
      pf.longPollSense(device_id);
    }
    
    // Send the values from input feeds every minute or so
    if (millis() - timeRef > 60000) {
      
      Serial.println("POST value for feed temperature-sensor");
      tempVal = readTemp();
      dtostrf(tempVal, 3, 1, buffer);
      Serial.println("Posting in feed temperature-sensor");
      if(pf.postSense(feed_41206, buffer)) {
        Serial.println("POST in feed temperature-sensor succeeded");
      } else {
        Serial.println("POST in feed temperature-sensor failed");
      }
      
      
      
      Serial.println("POST value for feed light-sensor");
      val = readLight();
      sprintf(buffer,"%d",val);
      Serial.println("Posting in feed light-sensor");
      if(pf.postSense(feed_41207, buffer)) {
        Serial.println("POST in feed light-sensor succeeded");
      } else {
        Serial.println("POST in feed light-sensor failed");
      }
      
    
      timeRef = millis();
    }
    
    delay(200);
}

/* read temp three times and create average */
float readTemp() {
  float val = sensors.getTempCByIndex(0);
  delay(500);
  val += sensors.getTempCByIndex(0);
  delay(500);
  val += sensors.getTempCByIndex(0);
  val /= 3.0;
  return val;
}

/** read light three times and create average */
int readLight() {
   int light = analogRead(lightPin);
  delay(500);
  light +=  analogRead(lightPin);
  delay(500);
  light +=  analogRead(lightPin);
  light /= 3;
  return light;
}
