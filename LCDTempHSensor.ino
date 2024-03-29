/**
 * A little program I wrote in a day for fun. This program is meant to be used with a side project I've been working on:
 * an SD card reader, a 1602 LCD, an Arduino Mega, and a DHT temperature/humidity sensor.
 * The purpose is to collect data and log them to an SD card for easy analysis on a computer.
 * 
 * Written by Roshan Sundaram...and Giggy
 * 8-15-2022
 */

// Include relevant libraries
#include "DHT.h"
#include <LiquidCrystal.h>
#include <SD.h>
#include <SPI.h>

// Create file objects
File loggedTempData;
File loggedHumidityData;

// Relevant parameters for DHT sensor
#define DHTPIN 13 // Change to corresponding DHT sensor pin on board
#define DHTTYPE DHT11 // Change to corresponding DHT sensor model

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int recordingInterval = 2000; // time between readings in milliseconds
int elapsedTime = 0; // Variable to store elapsed time--do not alter

int desiredRunTime = 10; // Desired data collection time in hours

LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Create LCD object
DHT dht(DHTPIN, DHTTYPE); // Create DHT sensor object

void setup() {
  Serial.begin(9600); // Start the serial monitor
  dht.begin(); // Begin polling the DHT sensor
  lcd.begin(16,2); // Turn on the LCD and set to 16 columns, 2 rows (1602 display)

  // Initializing the temperature file
  loggedTempData = SD.open("loggedTempData.txt", FILE_WRITE); 
  loggedTempData.println("Data recorded every "); 
  loggedTempData.print(recordingInterval);

  // Initializing the humidity file
  loggedHumidityData = SD.open("loggedHumidityData.txt", FILE_WRITE);
  loggedHumidityData.println("Data recorded every "); 
  loggedHumidityData.print(recordingInterval);

  // Notify user of data collection status
  lcd.print("Collection On");
  lcd.setCursor(0,1);
  lcd.print("Interval: ");
  lcd.print(recordingInterval);
  lcd.print("ms");
}

// Conversion between milliseonds and hours
int milliInHours = 3600000 * desiredRunTime;

void loop() {
  
  // Declare and initialize variables to store temperature and humidity
  float h = dht.readHumidity();
  float f = dht.readTemperature(true); // "true" indicates the use of Fahrenheit as units

  // Print temperatures to the Arduino serial monitor. This block should be commented out when module is deployed
  Serial.print(F("Humidity: "));
  Serial.print(h); 
  Serial.print(F("%  Temperature: "));
  Serial.print(f);
  Serial.println(F("°F"));

  // Write the temperature and humidity to the LCD with relevant information
  lcd.print("Temperature: ");
  lcd.print(f);
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(h);
  delay(recordingInterval); // Add delay between recordings
  
  // While the elapsed time is less than the desired time in milliseconds, write data
  // to relevant files on SD card
  while (elapsedTime < milliInHours) {
    loggedTempData.println(f);
    loggedHumidityData.println(h);
    
    elapsedTime+=recordingInterval; // Increment the elapsed time by the amount of time between recordings
  }

  // If the elapsed time has exceeded the desired time, close both data files one minute after 
  // the conclusion of data collection
  if ((elapsedTime > milliInHours) && (abs(milliInHours-elapsedTime) < 60000)) {
    loggedTempData.close();
    loggedHumidityData.close();
    lcd.print("Recording");
    lcd.setCursor(0,1);
    lcd.print("Completed"); // Notify the user of the closure of the SD card files
  }
  
  lcd.clear(); // Clear the LCD to make way for a fresh set of temperature and humidity readings

  // NOTE: data collection continues after the files on the SD card are closed. Those data are just written
  // to the serial monitor and/or the LCD for the duration of the collection period
  
}
