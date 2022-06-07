#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//Defining constants for the rest of the code
#include <max6675.h>

//LED PINS
const byte led_green = 13; //GREEN
const byte led_yellow = 12; //YELLOW
const byte led_red = 14; //RED

//temperature of the oil OIL
const byte danger_temp_oil = 80;
const byte max_temp_oil = 100;
float current_avg_oil = 0;
float printed_avg_oil = 0;

//temp CYLINDER HEAD
const short danger_temp_head = 160;
const short max_temp_head = 200;
float current_avg_head = 0;
float printed_avg_head = 0;

//time intervals
const short interval = 300; // buffer time for MAX6675
unsigned long previous_millis = 0;
const byte number_of_readings = 6;

//oil sensors
const byte thermo_oil_CLK = 5; //12;
const byte thermo_oil_CS = 17; //14;
const byte thermo_oil_DO = 16; //27;
float temp_oil = 69;

//for sensor 2
const byte thermo_head_CLK = 4; //26;
const byte thermo_head_CS = 2; //25;
const byte thermo_head_DO = 15; //33;
float temp_head = 69;

// Creating Thermopar objects
MAX6675 thermocouple_oil(thermo_oil_CLK, thermo_oil_CS, thermo_oil_DO);
MAX6675 thermocouple_head(thermo_head_CLK, thermo_head_CS, thermo_head_DO);

int i = 0;

void setup()
{

  Serial.begin(115200);
  SerialBT.begin("ECU-Fusca");
  SerialBT.println("The device started, now you can pair it with bluetooth!");
  SerialBT.println();

  pinMode(led_green, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_yellow, OUTPUT);

  //TO SHOW THAT ALL LEDS ARE WORKING
  digitalWrite(led_green, LOW);
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_red, HIGH);
  delay(1000);
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, HIGH);
  delay(1000);
  digitalWrite(led_green, LOW);
  digitalWrite(led_yellow, HIGH);
  delay(1000);
  digitalWrite(led_yellow, LOW);
  delay(1000);

}

void loop()
{
  
  unsigned long current_millis = millis();

  if(current_millis - previous_millis >= interval){

    previous_millis = current_millis;
    temp_oil = thermocouple_oil.readCelsius();
    temp_head = thermocouple_head.readCelsius();
    current_avg_oil = current_avg_oil + temp_oil/number_of_readings;
    current_avg_head = current_avg_head + temp_head/number_of_readings;
    i++;

  }
  
  if(i >= number_of_readings){

    i = 0;
    printed_avg_oil = current_avg_oil;
    current_avg_oil = 0;
    printed_avg_head = current_avg_head;
    current_avg_head = 0;
    
  }

  SerialBT.print(temp_oil);
  SerialBT.print(",");
  SerialBT.print(temp_head);
  SerialBT.print(",");
  SerialBT.println();
  Serial.print(temp_oil);
  Serial.print(",");
  Serial.print(temp_head);
  Serial.print(",");
  Serial.println();
  
  if (printed_avg_oil > max_temp_oil || printed_avg_head > max_temp_head)
  {
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green, LOW);
    digitalWrite(led_yellow, LOW);
  }
  else if (printed_avg_oil > danger_temp_oil || printed_avg_head > danger_temp_head)
  {
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, LOW);
    digitalWrite(led_yellow, HIGH);
  }
  else
  {
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, HIGH);
    digitalWrite(led_yellow, LOW);
  }
}