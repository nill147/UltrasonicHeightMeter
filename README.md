# UltrasonicHeightMeter(ESP8266 + OLED)

A smart height measurement system built using ESP8266, HC-SR04 ultrasonic sensor, and an OLED display.
It detects a person standing below the sensor, measures height automatically, and shows the result in feet, inches, and centimeters with buzzer and LED feedback.

---

🚀 Features

 🟢 Button-controlled start - press once to begin measurement
 📡 Ultrasonic distance sensing using HC-SR04
 ⏱️ 5-second countdown with tone and LED blink each second
 🧠 Automatic height calculation in ft/in/cm
 🖥️ Real-time OLED display for status, countdown, and results
 🔔 Buzzer feedback for progress and completion
 💡 Error detection when no object is found or out of range

---

⚙️ Components Used

| Component         | Model             |
| ----------------- | ----------------- |
| Microcontroller   | ESP8266 NodeMCU   |
| Ultrasonic Sensor | HC-SR04           |
| OLED Display      | SSD1306 0.96" I2C |
| Push Button       | Tactile switch    |
| Buzzer            | Passive           |
| LED               | 5mm any color     |

---

🔌 Pin Configuration

| Function       | Pin |
| -------------- | --- |
| Trig (HC-SR04) | D5  |
| Echo (HC-SR04) | D6  |
| Button         | D7  |
| Buzzer         | D8  |
| LED            | D4  |
| OLED SDA       | D2  |
| OLED SCL       | D1  |

---

🧮 Working Principle

1. Press the button to start.
2. The system checks for an object (person) below the sensor.
3. A 5-second countdown begins - LED blinks and buzzer beeps each second.
4. After countdown, it measures the final distance.
5. Calculates height using:

   ```
   height = sensorHeightCM - measuredDistance
   ```
6. Displays results on OLED and Serial Monitor.

---

⚠️ Notes

 Adjust `sensorHeightCM` in code to match your sensor's actual height from the floor.
 Ensure proper 3.3V operation - HC-SR04's Echo pin may need a voltage divider for ESP8266 safety.
 OLED I2C address is usually `0x3C`.

---

🧾 License

This project is licensed under the MIT License - free to use, modify, and share.

---
