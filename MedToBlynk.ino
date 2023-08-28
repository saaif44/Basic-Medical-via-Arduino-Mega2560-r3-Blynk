#define BLYNK_TEMPLATE_ID           "TMPL6aeU6PxxH"
#define BLYNK_TEMPLATE_NAME         "led control"
#define BLYNK_AUTH_TOKEN            "qxa4YGHbWrjgC8eM_2i2fT4Tjuzxwdtj"

#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>


char ssid[] = "Sss";
char pass[] = "KiChaoBro**";

#define EspSerial Serial3
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

BlynkTimer timer;

int ledPin = 7;
bool ledState = false; // Initial LED state is off

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
  
  timer.setInterval(1000L, checkButtonState);
}


void checkButtonState()
{
  Blynk.virtualWrite(V2, ledState); // Sync button state with actual LED state
}

BLYNK_WRITE(V2)
{
  Serial.println("Button state changed"); // Debug print
  ledState = param.asInt();
  digitalWrite(ledPin, ledState);
}


void loop()
{
  Blynk.run();
  timer.run();
}
