#include <Arduino.h>
#include <U8g2lib.h>
#include "HX711.h" //HX711로드셀 엠프 관련함수 호출
#define calibration_factor -7050.0 // 로드셀 스케일 값 선언
#define DOUT1  3 //엠프 데이터 아웃 핀 넘버 선언
#define CLK1  2  //엠프 클락 핀 넘버 
#define DOUT2  5 //엠프 데이터 아웃 핀 넘버 선언
#define CLK2  4  //엠프 클락 핀 넘버   
#define DISTANCE  3.0  //무게센서 간격(cm)   
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);
/* VCC -> 빨간색 GND, 갈색 VCC
 * RS/E -> 검정색 RS(CS) , 흰색 R/W(DATA), 회색 E(clock)
 * RST -> 초록색 PSB , 갈색 RST(reset)
 * BLK -> 주황색 BLA , 노란색 BLK
 */


HX711 scale1(DOUT1, CLK1); //엠프 핀 선언 
HX711 scale2(DOUT2, CLK2); //엠프 핀 선언 
const int rainSensor = A4;

void setup(void) {
  
  u8g2.begin();
  u8g2.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
  Serial.begin(9600);
  scale1.set_scale(calibration_factor);  //스케일 지정 
  scale1.tare();  //스케일 설정
  scale2.set_scale(calibration_factor);  //스케일 지정 
  scale2.tare();  //스케일 설정
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

void senseWeight() {
  unsigned long start_time, end_time;
  int duration = 0;
  int weightValue2 = 0;
 
  int weightValue1 = scale1.get_units();
  Serial.print("wegiht1: ");
  Serial.println(weightValue1);
  if (weightValue1 >=1){
    start_time = millis();
    while(weightValue2<1){
      int weightValue2 = scale2.get_units();
      Serial.print("wegiht2: ");
      Serial.println(weightValue2);
    }
    end_time = millis();
    duration = end_time-start_time/1000;
    double speed = DISTANCE / duration;
    Serial.print("speed: ");
    Serial.println(speed);
    
  }
  delay(100);
   
}
void loop() {
  // 비 감지 센서 값 읽기
  int rainValue = analogRead(rainSensor);
  senseWeight();
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

  delay(100);  // 1초 대기 후 다시 반복
}
