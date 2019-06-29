#include <Arduino.h>
#include <BMP280.h>

// Datasheet: https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf

BMP280 bmp;
float baselinePressure = 0;

float determineBaselinePressure(){
  //try to smooth out the pressure altitude readings
  //first get some bogus numbers, try to 'wake up' the sensor
  float pressure, temperature;
  for(int i = 0; i < 10; i ++){
    bmp.readSensor(&temperature, &pressure);
    delay(20);
  }
  //now start running tally of pressure
  //assume no significant changes in temperature
  float pressureBaseline = pressure;
  for(int i = 0; i < 50; i ++){
    bmp.readSensor(&temperature, &pressure);
    pressureBaseline += pressure;
    Serial.println(pressure);
    pressureBaseline /= 2.0;
    delay(20);
  }
  //return results
  return pressureBaseline;
}


float estimate_altitude(float cur_pressure, float baseline_pressure){
  float altitude; // in Si units for Pascal
  baseline_pressure /= 100;
  cur_pressure /= 100;
  altitude = 44330 * (1.0 - pow(cur_pressure / baseline_pressure, 0.1903));
  return altitude;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    delay(2000);
    Serial.println("Beginning!");

    if(bmp.begin() == 0){
      Serial.println("Connected");
    }
    else{
      Serial.println("Failed to connect!");
      while(true);
    }
    bmp.setFilter(BMP280::FILTER_16);
    delay(500);
    baselinePressure = determineBaselinePressure();
}

void loop() {
    // put your main code here, to run repeatedly:

    long start = micros();
    float pressure, temperature;
    bmp.readSensor(&temperature, &pressure);
    long end = micros();
    Serial.println("Temp: " + String(temperature));
    Serial.println("Pressure: " + String(pressure));
    Serial.println("Altitude: " + String(estimate_altitude(pressure, baselinePressure)));
    Serial.println("Baseline: " + String(baselinePressure));
    Serial.println(end - start);
    delay(50);

}
