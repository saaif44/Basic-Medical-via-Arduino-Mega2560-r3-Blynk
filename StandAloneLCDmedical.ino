#include<MemoryFree.h>
#include<EEPROM.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "LiquidCrystal_I2C.h"

#define LCD_ADDRESS 0x27 // Change this if your LCD address is different
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength = 100; // data length
int32_t spo2; // SPO2 value
int8_t validSPO2; // indicator to show if the SPO2 calculation is valid
int32_t heartRate; // heart rate value
int8_t validHeartRate; // indicator to show if the heart rate calculation is valid

byte pulseLED = 11; // Must be on PWM pin
byte readLED = 13; // Blinks with each data read


int beatAvg = 0, sp02Avg = 0;

enum DisplayMode {
  TEMPERATURE,
  BPM,
  HEARTRATE,
  SPO2
};

DisplayMode currentDisplay = TEMPERATURE;
unsigned long displayTimer = 0;

void setup() {
  Serial.begin(115200);
  
  lcd.init();
  lcd.backlight();

  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

}

void loop() {
   long irValue = particleSensor.getIR();

  // Dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
  for (byte i = 25; i < 100; i++) {
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25] = irBuffer[i];
  }

  // Taking 25 sets of samples before calculating the heart rate.
  for (byte i = 75; i < 100; i++) {
    while (particleSensor.available() == false)
      particleSensor.check();

    digitalWrite(readLED, !digitalRead(readLED));

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  float temperature = particleSensor.readTemperature() ;
  float temperatureF = particleSensor.readTemperatureF() ;

////Algo of averaging HeartRate
if (validHeartRate == 1 && heartRate > 50 && heartRate < 180 ) {  
      beatAvg =80;
      beatAvg = (beatAvg+heartRate)/2;
  } 
  if (irValue < 10000){
    heartRate=0;
    beatAvg = (beatAvg+heartRate)/2;
  }
 //BPM ALGORITHM
  int sys1=120;
  int dias1=80;
  int sys;
  int dias;
 if (beatAvg>80 && beatAvg<100){
   sys = beatAvg+30;          //beatAVG = 80
   dias = beatAvg -15;
 }
 else if (beatAvg>100 && beatAvg<120){
   sys = beatAvg+5;          //beatAVG = 110
   dias = beatAvg-30;
 }
 sys=(sys+sys1)/2;
 dias=(dias + dias1) /2;

//Algo of averaging sp02
  if (validSPO2 == 1 && spo2 > 0 ) {
    sp02Avg = 98;  
      sp02Avg = (sp02Avg+spo2)/2;
  } 
  else if (irValue < 10000){
    spo2=0;
    sp02Avg = (sp02Avg+spo2)/2;
  }
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();
    currentDisplay = static_cast<DisplayMode>((static_cast<int>(currentDisplay) + 1) % 4);
  }

  lcd.clear();
  lcd.setCursor(0, 0);



  if (irValue > 6500) {
    switch (currentDisplay) {
      case TEMPERATURE:
        lcd.print("Temp: " + String(temperatureF) + "F");
        break;
      case BPM:
        lcd.print("BPM: " + String(sys) + "/" + String(dias));
        break;
      case HEARTRATE:
        lcd.print("HR: " + String(beatAvg) + " bpm");
        break;
      case SPO2:
        lcd.print("SPO2: " + String(sp02Avg) + "%");
        break;
    }
  } else {
    lcd.print("PlaceYourFinger!");
  }
  Serial.print("Temp:");
  Serial.print(temperatureF);
  Serial.print(" IR=");
  Serial.print(irValue);
  Serial.print(", SPO2AVG=");
  Serial.print(sp02Avg);
  Serial.print(",AVG hr=");
  Serial.print(beatAvg);
  Serial.print(", BPM=");
  Serial.print(heartRate);
  Serial.print(", BloodPressure=");
  Serial.print(sys);
  Serial.print("/");
  Serial.print(dias);
  Serial.print(", SPO2=");
  Serial.print(spo2);
    
    if (irValue < 5000)
    Serial.print(" No finger?");
    Serial.println();
}
