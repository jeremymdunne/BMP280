#include <Arduino.h>
#include <BMP280.h>

// Datasheet: https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf

BMP280 bmp;
float baselinePressure = 0;

float determineBaselinePressure(){
  //try to smooth out the pressure altitude readings
  //first get some bogus numbers, try to 'wake up' the sensor
  for(int i = 0; i < 5; i ++){
    bmp.getTemperature();
    bmp.getPressure();
    delay(10);
  }
  //now start running tally of pressure
  //assume no significant changes in temperature
  float pressureBaseline = bmp.getPressure();
  for(int i = 0; i < 50; i ++){
    pressureBaseline += bmp.getPressure();
    pressureBaseline /= 2.0;
    delay(10);
  }
  //return results
  return pressureBaseline;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    /*
    pinMode(PA8, OUTPUT);
    while(true){
      digitalWrite(PA8,!digitalRead(PA8));
      delay(1000);
    }
    */
    delay(2000);
    Serial.println("Beginning!");

    if(bmp.begin() == 0){
      Serial.println("Connected");
    }
    else{
      Serial.println("Failed to connect!");
      while(true);
    }
    bmp.setFilter(BMP280::Filter_16);
    delay(50);
    baselinePressure = determineBaselinePressure();
}

void loop() {
    // put your main code here, to run repeatedly:

    long start = micros();
    Serial.println("Temp: " + String(bmp.getTemperature()));
    Serial.println("Pressure: " + String(bmp.getPressure()));
    Serial.println("Altitude: " + String(bmp.getAltitudeFromBaselinePressure(baselinePressure)));
    Serial.println(micros() - start);
    delay(50);

}
