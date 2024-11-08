#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

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

void setup(void) {
  
  u8g2.begin();
  u8g2.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
  Serial.begin(9600);
  pinMode(A4,INPUT);      // rain sensor
}

void loop(){
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
  delay(1000);
}
