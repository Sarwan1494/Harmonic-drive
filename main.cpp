

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>

#define ENABLE 1
#define DISABLE 0

File myFile;

// Arduino nano - CS pin - 10
// Arduino Uno - CS pin - 4
const int chipSelect = 10;
const uint8_t button_pin = 9;

char dataStr[100] = "";
char buffer[7];

float battery_voltage;
float shunt_voltage;
bool button_state;

uint16_t current_time, dt;
const uint16_t sample_rate = 500; // milliseconds
int num_of_samples = 10;

int count = 0;
int previous_time = 0;


void setup()
{

  Serial.begin(9600);

  pinMode(button_pin,INPUT_PULLUP);

  // strtol();

  delay(500);

  // Checking for SD card and initializing
  if (SD.begin(chipSelect))
  {
    Serial.println("SD card is present and ready");
  }
  else
  {
    Serial.println("SD card is missing or failure");
    while (1); // halt program
  }

  // Write csv headers to file
  Serial.println("Opening File");

  // clear out the old data
  if (SD.exists("csv.txt"))
  {
    Serial.println("Removing csv.txt");
    SD.remove("csv.txt");
    Serial.println("Done");
  }
  else
  {
    Serial.println("csv.txt does not exist");
  }
  delay(1000);
  myFile = SD.open("csv.txt", FILE_WRITE);
  delay(1000);
  if (myFile)
  {
    Serial.println("Writing headers to csv.txt");
    myFile.println("Time,Battery voltage,Shunt voltage");
    myFile.close();
    Serial.println("Headers written");
  }
  else
  {
    Serial.println("Error opening csv.txt");
  }
}

void loop()
{

  if (DISABLE)       //debugging purpose - to stop logging data
  {

    myFile = SD.open("csv.txt", FILE_WRITE);
    delay(500); // delay for the file to open properly

    if (myFile)
    {
      Serial.println("File opened properly");
    }

    // Getting the current time to calculate the sampling rate
    previous_time = millis();
    // dt = current_time - previous_time;
    // Serial.println("Current time set from milli function");

    // while((button_state = digitalRead(button_pin))){

    while (1)
    {
      // Serial.println("Entered while loop");
      // while (((dt = current_time - previous_time) >= sample_rate) && (count < num_of_samples))

      if ((millis() - previous_time) >= sample_rate)
      {

        previous_time = millis();

        dataStr[0] = 0;

        // Read voltage values
        battery_voltage = analogRead(A0);
        shunt_voltage = analogRead(A1);

        // Convert to float to string
        ltoa(previous_time, buffer, 10);
        strcat(dataStr, buffer);
        strcat(dataStr, ",");

        dtostrf(battery_voltage, 5, 2, buffer);
        strcat(dataStr, buffer);
        strcat(dataStr, ",");

        dtostrf(shunt_voltage, 5, 2, buffer);
        strcat(dataStr, buffer);
        strcat(dataStr, ","); // terminate the string with a null character

        if (myFile)
        {
          Serial.println("Writing voltage values to csv.txt");
          // Serial.println(dt);
          myFile.println(dataStr);
        }
        else
        {
          Serial.println("error opening csv.txt");
          break;
        }
        count++;
        // Serial.println(count);
      }    

      if (count >= num_of_samples)
      {
        Serial.print(count);
        Serial.println(" Voltage values written to SD card");
        break;
      }
    }

    myFile.close();
    Serial.println("File closed");
  }

  if (DISABLE) // Reading the file
  {
    myFile = SD.open("csv.txt");
    delay(1000);

    // Reading the file and writing to serial monitor
    if (myFile)
    {
      while (myFile.available())
      {
        Serial.write(myFile.read());
      }
      myFile.close();
    }
    else
    {
      Serial.println("error opening datalog.txt");
    }
  }

  button_state = digitalRead(button_pin);
  Serial.println("Program halted");
  while (1)
    ; // halt the program
}