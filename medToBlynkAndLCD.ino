//              Connecting Blynk App Information 
//              These info needs to held at first 
//--------------------------------------------------------------------------//
#define BLYNK_TEMPLATE_ID           "--------------------"
#define BLYNK_TEMPLATE_NAME         "---------------------------"
#define BLYNK_AUTH_TOKEN            "-------------------------------------"

#define BLYNK_PRINT Serial




//  All the Libraries that we used
//  Sometimes there needs to use MemoryFee And EEPROM library 
// #include<MemoryFree.h>
// #include<EEPROM.h>
//--------------------------------------------------------------------------//
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "LiquidCrystal_I2C.h"
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

//    Wifi Info of which network we are using 
//           your will be different
//-------------------------------------------------------------------------//
char ssid[] = "your wifi network name";
char pass[] = "that network password";

//    Our Mega is in Serial
//    wifi is in serial1
//    
#define EspSerial Serial3
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

BlynkTimer timer;


//State of Finger and commanding to place fingerState variables
int ledPin = 53;
int fingerLedPin = 49; 
bool ledState = false; // Initial LED state is off
bool fingerState = false;


#define LCD_ADDRESS 0x27 // Change this if your LCD address is different
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

MAX30105 particleSensor;

//Max sensor code below
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


//we are not using this pwm pin to show pulse 
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


void setup()
{
  Serial.begin(115200);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass);
  
  // Print Blynk connection status
  if (Blynk.connected()) {
    Serial.println("Connected to Blynk");
  } else {
    Serial.println("Not connected to Blynk");
  }
  
  pinMode(ledPin, OUTPUT);
  pinMode(fingerLedPin, OUTPUT);
  
  timer.setInterval(1000L, checkButtonState);
  timer.setInterval(1000L, checkFingerState);

  lcd.init();
  lcd.backlight();

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  //controlling Max Sensor module ranging
  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

}

///to check the button on Virtual Pin
void checkButtonState()
{
  Blynk.virtualWrite(V2, ledState); // Sync button state with actual LED state
}

void checkFingerState()
{
  Blynk.virtualWrite(V10, fingerState); // Sync button state with actual LED state
}


BLYNK_WRITE(V2)
{
  Serial.println("Button state changed"); // Debug print
  ledState = param.asInt();
  digitalWrite(ledPin, ledState);
}

BLYNK_WRITE(V10)
{
  Serial.println("Finger state changed"); // Debug print
   fingerState = param.asInt();
  digitalWrite(fingerLedPin, fingerState);
}

void loop()
{
  Blynk.run();
  timer.run();

  long irValue = particleSensor.getIR();

  ///////..........Algorithm of Ir Calculation and Algorithm of refining Parameters Calculation
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
      
     fingerState=true;
     digitalWrite(fingerLedPin, HIGH);
      Blynk.virtualWrite(V10, 1); 

    switch (currentDisplay) {

      

      case TEMPERATURE:
        lcd.print("Temp: " + String(temperatureF) + "F");
        Blynk.virtualWrite(V4, temperatureF);  // Send temperature to Blynk gauge
        break;
      case BPM:
        lcd.print("BPM: " + String(sys) + "/" + String(dias));
        Blynk.virtualWrite(V5, sys);  // Send systolic BP to Blynk gauge
        Blynk.virtualWrite(V5 + 1, dias);  // Send diastolic BP to Blynk gauge
        break;
      case HEARTRATE:
        lcd.print("HR: " + String(beatAvg) + " bpm");
        Blynk.virtualWrite(V7, beatAvg);  // Send heart rate to Blynk gauge
        break;
      case SPO2:
        lcd.print("SPO2: " + String(sp02Avg) + "%");
        Blynk.virtualWrite(V8, sp02Avg);  // Send SPO2 to Blynk gauge
        break;
    }
  }

  else {
    fingerState=false;
    Blynk.virtualWrite(V10, 0); 
    digitalWrite(fingerLedPin, LOW);

    lcd.print("PlaceYourFinger!");
      
      if (ledState==true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Place Please");
    }
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


