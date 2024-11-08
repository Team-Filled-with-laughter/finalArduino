#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

const int rainSensor = A4;

void setup(void) {
  
  u8g2.begin();
  u8g2.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
  Serial.begin(9600);
  pinMode(rainSensor,INPUT);      // rain sensor
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

void loop() {
  // 비 감지 센서 값 읽기
  int rainValue = analogRead(rainSensor);

  // 비가 내리면 (센서 값이 700 미만일 때)
  if (rainValue < 700) {
    u8g2.firstPage();        // 새로운 페이지 시작
    do {
      draw2();               // "비가 내리니 안전 운행하세요" 메시지 출력
    } while (u8g2.nextPage()); // 화면 업데이트
  } else {
    u8g2.firstPage();        // 새로운 페이지 시작
    do {
      draw1();               // "즐거운 하루 되세요!" 메시지 출력
    } while (u8g2.nextPage()); // 화면 업데이트
  }

  delay(1000);  // 1초 대기 후 다시 반복
}
