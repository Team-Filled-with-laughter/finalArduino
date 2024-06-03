#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

const int Sigpin1 = 8;
const int Sigpin2 = 12;
const int speed = 9;
const int In1 = 5;
const int In2 = 6;
const int overSpeed = 5;
const int Address = 0x50;
const long speedInterval = 100; // 0.1s마다 측정

bool isOverspeed = false;
bool isEmergency = false;

unsigned long previousMillis = 0;
unsigned long overSpeedStartTime = 0;

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

void setup() {
  u8g2.begin();
  u8g2.enableUTF8Print();
  Serial.begin(9600);
  pinMode(Sigpin1, INPUT);
  pinMode(Sigpin2, INPUT);
  pinMode(speed, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
}

void drawMessage(const char* line1, const char* line2) {
  u8g2.setFont(u8g2_font_unifont_t_korean1);
  u8g2.setFontDirection(0);
  u8g2.setCursor(20, 20);
  u8g2.print(line1);
  u8g2.setCursor(40, 40);
  u8g2.print(line2);
}

int measureSpeed(int pin) {
  while (digitalRead(pin));
  while (!digitalRead(pin));
  unsigned long T = pulseIn(pin, HIGH) + pulseIn(pin, LOW);
  double frequency = 1.0 / T;
  return int((frequency * 1e6) / 44.0);
}

void speedCheck() {
  int v1 = measureSpeed(Sigpin1);
  int v2 = measureSpeed(Sigpin2);

  char s[20];
  sprintf(s, "v1 Speed: %3d km/h", v1);
  Serial.println(s);

  sprintf(s, "v2 Speed: %3d km/h", v2);
  Serial.println(s);

  isOverspeed = (v1 >= overSpeed || v2 >= overSpeed);
}

void actuatorDown() {
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  analogWrite(speed, 255);
}

void actuatorUp() {
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  analogWrite(speed, 255);
}

void detectEmergency() {
  if (Serial.available() > 0) {
    char received = Serial.read();
    Serial.print("Received: ");
    Serial.println(received);

    isEmergency = (received == '1');
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= speedInterval) {
    previousMillis = currentMillis;

    speedCheck();
    detectEmergency();

    if (isOverspeed && !isEmergency) {
      overSpeedStartTime = currentMillis;
      Serial.println("Overspeed!!!");
      actuatorDown();
    }

    if (isOverspeed && currentMillis - overSpeedStartTime >= 5000) {
      actuatorUp();
    }
  }

  u8g2.firstPage();
  do {
    if (analogRead(A4) < 700) {
      drawMessage("비가 내리니", "안전 운행하세요");
    } else {
      drawMessage("즐거운 하루", "되세요!");
    }
  } while (u8g2.nextPage());
}
