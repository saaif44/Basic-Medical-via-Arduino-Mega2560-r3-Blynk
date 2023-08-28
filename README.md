# Basic-Medical-via-Arduino-Mega2560-r3-Blynk
This repository contains the code and resources for a Medical Information Reading System that utilizes the MAX30102 Pulse Oximeter and Heart-Rate Sensor in conjunction with an Arduino "Mega +WiFi R3 ATmega2560+ESP8266 CH340G" and blynk Platform.

# Medical Information Reading System with MAX30102 and Blynk

## Overview
This repository contains the code and resources for a Medical Information Reading System that utilizes the MAX30102 Pulse Oximeter and Heart-Rate Sensor in conjunction with an Arduino Mega and ESP8266 module. The system collects and processes medical data such as heart rate, blood oxygen saturation (SpO2), and more, and displays the real-time data on the Blynk app. Additionally, the system allows you to maintain a patient database through the Blynk cloud.

## Features
1. Accurate and reliable medical data collection using the MAX30102 sensor.
2. Real-time visualization of heart rate, SpO2, and other vital signs on the Blynk app.
3. Patient database management with data logging and historical analysis using Blynk's cloud capabilities.
4. Integration with LEDs for status indication and debugging.
5. Easy-to-use Blynk app interface for healthcare professionals and patients.
6. Hardware Requirements
7. Arduino Mega board Mega + ESP8266 WiFi R3 Module
9. MAX30102 ppg sensor
10. LEDs and resistors (for status indication - optional)
11. Wi-Fi network for internet connectivity
12. Software Requirements
13. Arduino IDE
14. Required libraries for the MAX30102 sensor, ESP8266 WiFi module, and Blynk app integration.
15. Libraries are Wire.h,MAX30105.h,spo2_algorithm.h,LiquidCrystal_I2C.h,ESP8266_Lib.h,BlynkSimpleShieldEsp8266.h

This is our Board Circuit Diagram but we used different Arduino Board 

![StandAloneLCDmedical Image](https://github.com/saaif44/Basic-Medical-via-Arduino-Mega2560-r3-Blynk/assets/41290426/d52dec39-0772-4728-a902-da0c7900c636)

We use This Board in our Project Cause this Board included ESP8266 and Wifi
![Mega +WiFi R3](https://github.com/saaif44/Basic-Medical-via-Arduino-Mega2560-r3-Blynk/assets/41290426/e02a61f0-104e-49de-9d0f-aa8cd052932b)

There is Two file of code 
- MedToBlynkAndLCD.ino code is for the board we used. also if the board is connect with esp8266 with wifi code need to slight change according to the hardware needs
- StandAloneLCDmedical.ino is for withour wifi just as the   

## There is a slight complication with the board that can be fixed 
The Esp8266 EEPROM get junked and make garbage tx/rx communication
If it gets junked then it needs to factory flash to resolve the issue

## Usage
- Setting up hardware components according to the wiring diagram provided in the repository.
- Installed the required libraries in your Arduino IDE.
- Open the provided Arduino sketch and configure your Wi-Fi credentials, Blynk authentication token, and other settings as necessary.
- Upload the sketch to your Arduino Mega.
- Create a Blynk account and project on the Blynk app.
- Configure Blynk widgets (Value Display, Gauge) to match the data being collected by the MAX30102 sensor.
- Run the project. The Blynk app will display real-time medical data, and you can also maintain a patient database using Blynk's data logging features.


## Contributions
Contributions to this project are welcome! If you have suggestions for improvements, bug fixes, or additional features, feel free to open an issue or submit a pull request.

## Credits
This project was developed by [Your Name] and [Collaborator's Names] as part of [Project/Class Name].

## License
This project is licensed under the MIT License.
