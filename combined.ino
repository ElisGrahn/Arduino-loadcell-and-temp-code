#define ARDUINO_H
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <HX711.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Temp wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4

void calibrate();

// Setup a oneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

HX711 scale;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

void setup() {

  // temp setup
  // start temp serial port
  Serial.begin(9600);

  // Start up the temp library
  sensors.begin();

  // Start up the loadcell library
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 128);

  calibrate();

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
  // by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop() {
  // Read temp
  sensors.requestTemperatures(); // Send the command to get temp

  // Print temp
  Serial.print("Temperature: ");
  Serial.println(sensors.getTempCByIndex(0));

  // Read and print weight
  Serial.print("Weight: ");
  Serial.println(scale.get_units(10), 1);

  Serial.print("\n");
  
  scale.power_down(); // Put the scale in sleep mode
  delay(2000);  // time to wait until next reading
  scale.power_up();   // Power the scale back up
}

void calibrate() {
  // Remove any calibration values and clear the scale
  scale.set_scale();
  scale.tare();

  // Prompt the user
  Serial.println("Add your known weight to the scale, enter the weight and press <Enter>");
  int userInput = -123;
  String inputString = "";
  // Loop until we receive an input (which will change the value of userInput
  while (userInput == -123) {
    // Read serial input:
    while (Serial.available() > 0) {
      int inputChar = Serial.read();
      if (isDigit(inputChar)) {
        // convert the incoming byte to a char and add it to the string:
        inputString += (char)inputChar;
      }
      // if you get a newline, print the string, then the string's value:
      if (inputChar == '\n') {
        userInput = inputString.toInt();
      }
    }
  }

  // Now get the reading from the scale
  float calReading = scale.get_units(10);

  Serial.print("Setting the cal to ");
  Serial.println(calReading / userInput);

  scale.set_scale(calReading / userInput);
}
