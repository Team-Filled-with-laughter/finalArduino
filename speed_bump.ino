#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <MsTimer2.h>
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

int measureSpeed(int pin) {
  unsigned long T = pulseIn(pin, HIGH, 100) + pulseIn(pin, LOW, 100); // 0.1s안에 HIGH값 안들어오면 0처리

  if (T != 0)
  {
    double frequency = 1.0 / T;
    int value = ((frequency * 1e6) / 44.0);

    if (value <= 120) 
    {
      return value;
    }
    else // 속도 이상값 
    {
      Serial.print("Velocity Outlier!")
      return 0;
    }
  }
  else { // 측정이 안된경우 
    Serial.print("No sensing!");
    return 0;
  }
}

void speedCheck1() {
  v1 = measureSpeed(Sigpin1);
  Serial.print("v1 Speed: ");
  Serial.print(v1);
  Serial.println(" km/h");
}

void speedCheck2() {
  v2 = measureSpeed(Sigpin2);
  Serial.print("v2 Speed: ");
  Serial.print(v2);
  Serial.println(" km/h");
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
  unsigned long currentMillis = millis();

  speedCheck1();
  speedCheck2();

  isOverspeed = (v1 >= overSpeed || v2 >= overSpeed) ? true : false; // 수정

  if (isOverspeed){ 
    detectEmergency(); // 과속이 아니면 사실상 어떤 객체가 지나가도 작동안하므로 과속일때만 객체를 인식하는 기능

    if (!isEmergency){
      overSpeedStartTime = currentMillis;
      Serial.println("Overspeed!!!");
      actuatorDown();
      while (currentMillis <= overSpeedStartTime + 3000); // delay 3초 (아직 코드상 delay방법으로 채택)
      actuatorUp();
      overSpeedStartTime = 0; // 액추에이터가 올라오면 시간 초기화
    }
  }
}
