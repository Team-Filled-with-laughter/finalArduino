#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <MsTimer2.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// 아두이노에서 인터럽트핀을 사용하려면 2,3번핀만 사용가능 (핀 번호 교체이유)
const int Sigpin1 = 2; // 원래 8번핀 
const int Sigpin2 = 3; // 원래 12번핀
const int speed = 9;
const int In1 = 5;
const int In2 = 6;
const int overSpeed = 5;
const int Address = 0x50;

bool isOverspeed = false;
bool isEmergency = false;

unsigned long overSpeedStartTime = 0;

int v1 = 0;
int v2 = 0;

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
  MsTimer2::set(500,playLcd);
  MsTimer2::start();
  attachInterrupt(digitalPinToInterrupt(Sigpin1), measurePulse1, RISING); // Sigpin1에 대한 인터럽트
  attachInterrupt(digitalPinToInterrupt(Sigpin2), measurePulse2, RISING); // Sigpin2에 대한 인터럽트
  actuatorUp();
}

void drawMessage(const char* line1, const char* line2, int cursorX) {
  u8g2.setFont(u8g2_font_unifont_t_korean1);
  u8g2.setFontDirection(0);
  u8g2.setCursor(20, 20);
  u8g2.print(line1);
  u8g2.setCursor(cursorX, 40);
  u8g2.print(line2);
}

void playLcd() {
u8g2.firstPage();
  do {
    if (analogRead(A4) < 700) {
      drawMessage("비가 내리니", "안전 운행하세요", 5);
    } else {
      drawMessage("즐거운 하루", "되세요!", 40);
    }
  } while (u8g2.nextPage());
}

void speedCheck1() {
  unsigned long T = pulseIn(Sigpin1, HIGH) + pulseIn(Sigpin1, LOW); // 0.1s안에 HIGH값 안들어오면 0처리

  if (T != 0)
  {
    double frequency = 1.0 / T;
    v1 = ((frequency * 1e6) / 44.0);

    if (value <= 120) 
    {
      Serial.print("v1 Speed: ");
      Serial.print(v1);
      Serial.println(" km/h");
    }
    else // 속도 이상값 
    {
      Serial.print("Velocity Outlier!")
    }
  }
  else 
  { // 측정이 안된경우 
    Serial.print("No sensing!");
  }
}

void speedCheck2() {
  unsigned long T = pulseIn(Sigpin2, HIGH) + pulseIn(Sigpin2, LOW); // 0.1s안에 HIGH값 안들어오면 0처리

  if (T != 0)
  {
    double frequency = 1.0 / T;
    v2 = ((frequency * 1e6) / 44.0);

    if (value <= 120) 
    {
      Serial.print("v2 Speed: ");
      Serial.print(v1);
      Serial.println(" km/h");
    }
    else // 속도 이상값 
    {
      Serial.print("Velocity Outlier!")
    }
  }
  else 
  { // 측정이 안된경우 
    Serial.print("No sensing!");
  }
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
    isEmergency = (received == '1') ? true : false; // 수정
  }
}

void loop() {

  isOverspeed = (v1 >= overSpeed || v2 >= overSpeed) ? true : false; // 수정

  if (isOverspeed){ 
    detectEmergency(); // 과속이 아니면 사실상 어떤 객체가 지나가도 작동안하므로 과속일때만 객체를 인식하는 기능

    if (!isEmergency){
      Serial.println("Overspeed!!!");
      actuatorDown();
      delay(3000);
      actuatorUp();
    }
  }
}
