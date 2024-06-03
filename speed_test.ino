#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

int v1;           // 첫 번째 센서의 순간 속도
int v2;           // 두 번째 센서의 순간 속도
int Sigpin1 = 8; // 첫 번째 센서의 신호 입력 핀
int Sigpin2 = 12; // 두 번째 센서의 신호 입력 핀
int speed = 9;
int In1 = 5;
int In2 = 6;
bool isOverspeed = false;
bool isEmergency = false;

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);
const int Address = 0x50;

int overSpeed = 5;
unsigned long previousMillis = 0;
const long speedInterval = 100;            // 0.1s마다 측정
unsigned long overSpeedStartTime = 0;

void setup() {
  u8g2.begin();
  u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
  Serial.begin(9600);
  pinMode(Sigpin1, INPUT);
  pinMode(Sigpin2, INPUT);
  pinMode(speed, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
}

void draw1(void)
{
  u8g2.setFont(u8g2_font_unifont_t_korean1);
  u8g2.setFontDirection(0);
  u8g2.setCursor(20, 20);
  u8g2.print("즐거운 하루");
  u8g2.setCursor(40, 40);
  u8g2.print("되세요!");
}

void draw2(void)
{
  u8g2.setFont(u8g2_font_unifont_t_korean1);
  u8g2.setFontDirection(0);
  u8g2.setCursor(20, 20);
  u8g2.print("비가 내리니");
  u8g2.setCursor(5, 40);
  u8g2.print("안전 운행하세요");
}

void speedCheck()
{
  unsigned long T1, T2;   // 주기
  double f1, f2;          // 주파수
  char s[20];             // Serial 출력 Length

  // 첫 번째 센서 측정
  while (digitalRead(Sigpin1));
  while (!digitalRead(Sigpin1));
  T1 = pulseIn(Sigpin1, HIGH) + pulseIn(Sigpin1, LOW); // 주기 측정
  f1 = 1 / (double)T1;                                // 주파수 측정
  v1 = int((f1 * 1e6) / 44.0);                         // 속도 측정

  sprintf(s, "v1 Speed: %3d km/h", v1);
  Serial.println(s); // Serial 출력

  // 두 번째 센서 측정
  while (digitalRead(Sigpin2));
  while (!digitalRead(Sigpin2));
  T2 = pulseIn(Sigpin2, HIGH) + pulseIn(Sigpin2, LOW); // 주기 측정
  f2 = 1 / (double)T2;                                // 주파수 측정
  v2 = int((f2 * 1e6) / 44.0);                         // 속도 측정

  sprintf(s, "v2 Speed: %3d km/h", v2);
  Serial.println(s); // Serial 출력

  if(v1 >= overSpeed || v2 >= overSpeed){
    isOverspeed = true;
  }
  else{
    isOverspeed = false;
  }
}

void actuatorDown(){
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  analogWrite(speed,255);
}

void actuatorUp(){
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  analogWrite(speed,255);
}

void detect(){
  if(Serial.available() > 0){
    char received = Serial.read();
    Serial.print("Received: ");
    Serial.println(received);

    if(received == '0'){
      isEmergency = false;
    } else if(received == '1'){
      isEmergency = true;
    }
  }
}

void loop() {
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= speedInterval){
    previousMillis = currentMillis;
    
    speedCheck();
    detect();

    if(isOverspeed && !isEmergency){
      overSpeedStartTime = currentMillis;
      
      Serial.print("Overspeed!!!");
      Serial.println("");
      actuatorDown();

    }

    if(currentMillis - overSpeedStartTime >= 5000){
        actuatorUp();
    }

  }
  
  if(analogRead(A4)<700){
    u8g2.firstPage();
    do {
      draw2();
    } while ( u8g2.nextPage() );
  }
  else{
    u8g2.firstPage();
    do {
      draw1();
    } while ( u8g2.nextPage() );
  }
  
}