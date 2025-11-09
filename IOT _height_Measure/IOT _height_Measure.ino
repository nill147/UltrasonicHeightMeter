#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TRIG_PIN D5
#define ECHO_PIN D6
#define BUZZER_PIN D8
#define BUTTON_PIN D7
#define LED_PIN D4

// I2C OLED pins use Wire default for ESP8266: SDA=D2, SCL=D1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const float sensorHeightCM = 189.0; // 6.2 feet
unsigned long presenceStartTime = 0;
bool objectDetected = false;
unsigned long lastBuzzTime = 0;
int secondsCounted = 0;
bool processStarted = false;

int buzzerTones[5] = {500, 600, 700, 800, 900}; // Different tones per second

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Active LOW
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is common address
    Serial.println("SSD1306 allocation failed");
    // If OLED fails, continue without blocking; user can still use serial
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Height Meter Ready");
    display.println("Press button to start");
    display.display();
  }
}

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout after 30ms
  float distanceCM = duration * 0.034 / 2;
  return distanceCM;
}

void showOLEDIdle() {
  if (!display.width()) return;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Height Meter");
  display.println();
  display.println("Status: Idle");
  display.println();
  display.println("Press button to start");
  display.display();
}

void showOLEDCounting(int secElapsed, float distance) {
  if (!display.width()) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Height Meter");
  display.println();
  display.print("Counting: ");
  display.print(secElapsed);
  display.println(" / 5s");
  display.println();
  display.print("Dist: ");
  if (distance <= 0 || distance > 10000) display.println("N/A");
  else {
    display.print(distance, 1);
    display.println(" cm");
  }
  display.display();
}

void showOLEDNoObject() {
  if (!display.width()) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Height Meter");
  display.println();
  display.println("No object detected");
  display.println("or out of range");
  display.display();
}

void showOLEDResult(int feet, float inchesDecimal, float cm) {
  if (!display.width()) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Height Measurement");
  display.println();
  display.print("Height: ");
  display.print(feet);
  display.print(" ft ");
  display.print(inchesDecimal, 1);
  display.println(" in");
  display.println();
  display.print("Centimeters: ");
  display.print(cm, 1);
  display.println(" cm");
  display.display();
}

void loop() {
  // Idle state
  if (!processStarted) {
    digitalWrite(LED_PIN, LOW);
    showOLEDIdle();
    if (digitalRead(BUTTON_PIN) == LOW) {
      processStarted = true;
      Serial.println("Measurement process started...");
      if (display.width()) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Measurement started");
        display.display();
      }
      delay(300); // Debounce
    }
    return;
  }

  float distance = readDistanceCM();

  if (distance > 2 && distance < sensorHeightCM - 3) {
    if (!objectDetected) {
      objectDetected = true;
      presenceStartTime = millis();
      lastBuzzTime = millis();
      secondsCounted = 0;
    } else {
      unsigned long now = millis();

      // Blink LED every 0.5s during countdown
      if ((now / 500) % 2 == 0) digitalWrite(LED_PIN, HIGH);
      else digitalWrite(LED_PIN, LOW);

      // Update OLED counting
      showOLEDCounting(secondsCounted, distance);

      // Tone every second
      if (now - lastBuzzTime >= 1000 && secondsCounted < 5) {
        tone(BUZZER_PIN, buzzerTones[secondsCounted]);
        delay(150);
        noTone(BUZZER_PIN);
        lastBuzzTime = now;
        secondsCounted++;
      }

      // After 5 seconds, measure and print
      if (now - presenceStartTime >= 5000) {
        // Validate final distance again
        float finalDist = readDistanceCM();

        if (finalDist > sensorHeightCM || finalDist < 2 || finalDist > 300) {
          Serial.println("No object detected or out of range.");
          showOLEDNoObject();
          // Fast blink LED for error
          for (int i = 0; i < 10; i++) {
            digitalWrite(LED_PIN, i % 2);
            delay(100);
          }
        } else {
          float objectHeightCM = sensorHeightCM - finalDist;
          if (objectHeightCM < 0) objectHeightCM = 0; // safety

          int feet = objectHeightCM / 30.48;
          float inchesDecimal = (objectHeightCM - (feet * 30.48)) / 2.54;

          Serial.print("Object Height: ");
          Serial.print(feet);
          Serial.print(" ft ");
          Serial.print(inchesDecimal, 1);
          Serial.print(" in | ");
          Serial.print(objectHeightCM, 1);
          Serial.println(" cm");

          showOLEDResult(feet, inchesDecimal, objectHeightCM);

          digitalWrite(LED_PIN, HIGH); // Solid LED = success
          delay(2000);
        }

        objectDetected = false;
        processStarted = false;
        digitalWrite(LED_PIN, LOW); // Reset LED
        delay(1000);
      }
    }
  } else {
    objectDetected = false;
  }

  delay(100);
}
