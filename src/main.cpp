#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#include <MAX6675Soft.h>

//LED PINS
const byte ledG = 13; //GREEN
const byte ledY = 12; //YELLOW
const byte ledR = 14; //RED

/*
MAX6675Soft(cs, so, sck)
cs  - chip select
so  - serial data output
sck - serial clock input
*/

MAX6675Soft thermocoupleoleo(2, 4, 15);
MAX6675Soft thermocouplecab(17, 5, 16);


//temperature of the oil OIL
const byte dangerTEMPoleo = 80;
const byte maxTEMPoleo = 100;

//temp CYLINDER HEAD
const byte dangerTEMPcab = 160;
const byte maxTEMPcab = 200;

//time intervals
unsigned long previousMillis = 0;
const long interval = 300;

//RPMS
const int maxRPM = 4000;
const int medRPM = 2000;
const int minRPM = 850;
float temperaturaoleo = 25;
float temperaturacab = 25;
int RPM = 0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  SerialBT.println("The device started, now you can pair it with bluetooth!");
  Serial.println();
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledY, OUTPUT);

  //TO SHOW THAT ALL LEDS ARE WORKING
  digitalWrite(ledG, LOW);
  digitalWrite(ledY, LOW);
  digitalWrite(ledR, HIGH);
  delay(1000);
  digitalWrite(ledR, LOW);
  digitalWrite(ledG, HIGH);
  delay(1000);
  digitalWrite(ledG, LOW);
  digitalWrite(ledY, HIGH);
  delay(1000);
  digitalWrite(ledY, LOW);

  thermocoupleoleo.begin();
  thermocouplecab.begin();

  while (thermocoupleoleo.getChipID() != MAX6675_ID)
  {
    SerialBT.println(F("MAX6675 Oleo error")); //(F()) saves string to flash & keeps dynamic memory free
    delay(5000);
  }
  SerialBT.println(F("MAX6675 Oleo OK"));

  
  while (thermocouplecab.getChipID() != MAX6675_ID)
  {
    SerialBT.println(F("MAX6675 Cabeçote error")); //(F()) saves string to flash & keeps dynamic memory free
    delay(5000);
  }
  SerialBT.println(F("MAX6675 Cabeçote OK"));

  SerialBT.println("oil,cylinderhead,");
  
  delay(1000);
}

void loop() {
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    float temperaturaoleo = thermocoupleoleo.getTemperature(thermocoupleoleo.readRawData());
    float temperaturacab = thermocouplecab.getTemperature(thermocouplecab.readRawData());
    SerialBT.print(temperaturaoleo);
    SerialBT.print(",");
    SerialBT.print(temperaturacab);
    SerialBT.print(",");
    SerialBT.println();
    Serial.print(temperaturaoleo);
    Serial.print(",");
    Serial.print(temperaturacab);
    Serial.print(",");
    Serial.println();
  }

  if (temperaturaoleo > maxTEMPoleo || temperaturacab > maxTEMPcab)
  {
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, LOW);
    digitalWrite(ledY, LOW);
  }
  else if (temperaturaoleo > dangerTEMPoleo || temperaturacab > dangerTEMPcab)
  {
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, LOW);
    digitalWrite(ledY, HIGH);
  }
  else
  {
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, HIGH);
    digitalWrite(ledY, LOW);
  }
}