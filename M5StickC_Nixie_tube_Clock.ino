// M5StickC Nixie tube Clock: 2019.07.21 
// Original: https://macsbug.wordpress.com/
// mods by Ewald:
// 20190721 addedd blinking dots in mode 3+4 to indicate seconds
// 20190731 addedd brightness control
//          addedd HH:MM mode with seconds bar

#include <M5StickC.h>
#include "vfd_18x34.c"
#include "vfd_35x67.c"

//############# brightness stuff
#define LCD_MAX_BRIGHTNESS      15
#define LCD_MIN_BRIGHTNESS      7
#define LCD_DEFAULT_BRIGHTNESS  10
#define ISR_DITHERING_TIME_MS   10

int BUTTON_PIN = 39;
unsigned long last_isr_time;
uint8_t lcd_brightness = LCD_DEFAULT_BRIGHTNESS;
boolean lcd_brightness_changed = true;    // will trigger initial setting of brightness to LCD_DEFAULT_BRIGHTNESS

void check_lcd_brightness_change() {
  if (lcd_brightness_changed) {
    M5.Axp.ScreenBreath(lcd_brightness);
    lcd_brightness_changed = false;
  }
} //check_lcd_brightness_change
  
void button_isr() {
  if (millis() - last_isr_time < ISR_DITHERING_TIME_MS) {
    return;
  }
  last_isr_time = millis();
    if (++lcd_brightness >= LCD_MAX_BRIGHTNESS) lcd_brightness = LCD_MIN_BRIGHTNESS;
    Serial.printf("lcd_set_brightness=%d\r\n", lcd_brightness);
    lcd_brightness_changed = true;
} //button_isr
//############# end brightness stuff


RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

int mode_ = 1; // 4:1Line(HH:MM) 3:2Lines(HH:MM+SS), 2:2Lines: yyyy,mm,dd,hh,mm,ss, 1:HH:MM+seconds bar

const uint8_t*n[] = { // vfd font 18x34
  vfd_18x34_0,vfd_18x34_1,vfd_18x34_2,vfd_18x34_3,vfd_18x34_4,
  vfd_18x34_5,vfd_18x34_6,vfd_18x34_7,vfd_18x34_8,vfd_18x34_9 };
const uint8_t*m[] = { // vfd font 35x67
  vfd_35x67_0,vfd_35x67_1,vfd_35x67_2,vfd_35x67_3,vfd_35x67_4,
  vfd_35x67_5,vfd_35x67_6,vfd_35x67_7,vfd_35x67_8,vfd_35x67_9 };
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


void setup(void){ 
  M5.begin();
  
  pinMode(M5_BUTTON_HOME, INPUT);                                           // mode control
  pinMode(BUTTON_PIN, INPUT | PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_isr, FALLING);  // brightness control

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setRotation(1);
  
  // rtc setup start ---------------------------------------------
  String pt = (__DATE__ " " __TIME__); // PC DATE TIME READ
  //000000000011111111112  Read data
  //012345678901234567890
  //Jun  6 2019 07:20:41
  
  char m1[3]; int  m2; // Month conversion ( Jun to 6 )
  (pt.substring(0,3)).toCharArray(m1,4);
  for (int mx = 0; mx < 12; mx ++) {
    if (strcmp(m1, monthName[mx]) == 0){m2 = mx + 1; break;}
  }
  RTC_DateTypeDef DateStruct;         // Month, Date, Year 
  DateStruct.Month   = m2;
  DateStruct.Date    = (pt.substring(4, 6)).toInt();
  DateStruct.Year    = (pt.substring(7,11)).toInt();
  M5.Rtc.SetData(&DateStruct);
  RTC_TimeTypeDef TimeStruct;         // Hours, Minutes, Seconds 
  TimeStruct.Hours   = (pt.substring(12,14)).toInt();
  TimeStruct.Minutes = (pt.substring(15,17)).toInt();
  TimeStruct.Seconds = (pt.substring(18,20)).toInt();
  M5.Rtc.SetTime(&TimeStruct);
  // rtc setup end -----------------------------------------------
  
} //setup
 
void loop(void){ 
  check_lcd_brightness_change();
  
  if(digitalRead(M5_BUTTON_HOME) == LOW){
      if (++mode_ == 5) mode_ = 1;
      M5.Lcd.fillScreen(BLACK);
      while (digitalRead(M5_BUTTON_HOME) == LOW) delay(10);         // wait for button release
      }

  switch(mode_) {
    case 1: vfd_1_line(); break;          // mm,ss
    case 2: vfd_2_line(); break;          // yyyy,mm,dd,hh,mm,ss
    case 3: vfd_3_line(); break;          // hh,mm,ss
    case 4: vfd_4_line(); break;          // hh,mm
  } //switch 

   delay(1000);                           // no need to go any faster
} //loop

void vfd_4_line(){                  // HH:MM
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Rtc.GetData(&RTC_DateStruct);
  int h1 = int(RTC_TimeStruct.Hours / 10 );
  int h2 = int(RTC_TimeStruct.Hours - h1*10 );
  int i1 = int(RTC_TimeStruct.Minutes / 10 );
  int i2 = int(RTC_TimeStruct.Minutes - i1*10 );
  int s0 = int(RTC_TimeStruct.Seconds );
  
  M5.Lcd.pushImage(  2,6,35,67, (uint16_t *)m[h1]);
  M5.Lcd.pushImage( 41,6,35,67, (uint16_t *)m[h2]);
  M5.Lcd.pushImage( 83,6,35,67, (uint16_t *)m[i1]);
  M5.Lcd.pushImage(121,6,35,67, (uint16_t *)m[i2]);
    if ((s0 % 2) == 0) {                                          // this will make the dots blink every second
      M5.Lcd.drawPixel( 79,28, ORANGE); M5.Lcd.drawPixel( 79,54,ORANGE);
      M5.Lcd.drawPixel( 79,27, YELLOW); M5.Lcd.drawPixel( 79,53,YELLOW);
      }
  else {
     M5.Lcd.drawPixel( 79,28, BLACK); M5.Lcd.drawPixel( 79,54,BLACK);
     M5.Lcd.drawPixel( 79,27, BLACK); M5.Lcd.drawPixel( 79,53,BLACK);
     }
} //vfd_4_line

void vfd_3_line(){                  // HH:MM+ss
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Rtc.GetData(&RTC_DateStruct);
  int h1 = int(RTC_TimeStruct.Hours / 10 );
  int h2 = int(RTC_TimeStruct.Hours - h1*10 );
  int i1 = int(RTC_TimeStruct.Minutes / 10 );
  int i2 = int(RTC_TimeStruct.Minutes - i1*10 );
  int s0 = int(RTC_TimeStruct.Seconds );
  int s1 = int(s0 / 10 );
  int s2 = int(s0 - s1*10 );
  
  M5.Lcd.pushImage(  2,0,35,67, (uint16_t *)m[h1]);
  M5.Lcd.pushImage( 41,0,35,67, (uint16_t *)m[h2]);
  M5.Lcd.pushImage( 83,0,35,67, (uint16_t *)m[i1]);
  M5.Lcd.pushImage(121,0,35,67, (uint16_t *)m[i2]);
  M5.Lcd.pushImage(120,45,18,34, (uint16_t *)n[s1]);
  M5.Lcd.pushImage(140,45,18,34, (uint16_t *)n[s2]);
    if ((s0 % 2) == 0) {                                    // this will make the dots blink every second
      M5.Lcd.drawPixel( 79,28, BLACK); M5.Lcd.drawPixel( 79,54,BLACK);
      M5.Lcd.drawPixel( 79,27, BLACK); M5.Lcd.drawPixel( 79,53,BLACK);
      }
  else {
      M5.Lcd.drawPixel( 79,28, ORANGE); M5.Lcd.drawPixel( 79,54,ORANGE);
      M5.Lcd.drawPixel( 79,27, YELLOW); M5.Lcd.drawPixel( 79,53,YELLOW);
     }  
  // if ( s1 == 0 && s2 == 0 ){ fade();}
} //vfd_3_line

void vfd_2_line(){          // yyyy,mm,dd + hh,mm,ss
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Rtc.GetData(&RTC_DateStruct);
  //Serial.printf("Data: %04d-%02d-%02d\n",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date);
  //Serial.printf("Week: %d\n",RTC_DateStruct.WeekDay);
  //Serial.printf("Time: %02d : %02d : %02d\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds);
  // Data: 2019-06-06
  // Week: 0
  // Time: 09 : 55 : 26
  int y1 = int(RTC_DateStruct.Year    / 1000 );
  int y2 = int((RTC_DateStruct.Year   - y1*1000 ) / 100 );
  int y3 = int((RTC_DateStruct.Year   - y1*1000 - y2*100 ) / 10 );
  int y4 = int(RTC_DateStruct.Year    - y1*1000 - y2*100 - y3*10 );
  int j1 = int(RTC_DateStruct.Month   / 10);
  int j2 = int(RTC_DateStruct.Month   - j1*10 );
  int d1 = int(RTC_DateStruct.Date    / 10 );
  int d2 = int(RTC_DateStruct.Date    - d1*10 );
  int h1 = int(RTC_TimeStruct.Hours   / 10) ;
  int h2 = int(RTC_TimeStruct.Hours   - h1*10 );
  int i1 = int(RTC_TimeStruct.Minutes / 10 );
  int i2 = int(RTC_TimeStruct.Minutes - i1*10 );
  int s1 = int(RTC_TimeStruct.Seconds / 10 );
  int s2 = int(RTC_TimeStruct.Seconds - s1*10 );
   
  M5.Lcd.pushImage(  0, 0,18,34, (uint16_t *)n[y1]); 
  M5.Lcd.pushImage( 19, 0,18,34, (uint16_t *)n[y2]);
  M5.Lcd.pushImage( 38, 0,18,34, (uint16_t *)n[y3]);
  M5.Lcd.pushImage( 57, 0,18,34, (uint16_t *)n[y4]);
  M5.Lcd.drawPixel( 77,13, ORANGE); M5.Lcd.drawPixel( 77,23,ORANGE);
  M5.Lcd.pushImage( 80, 0,18,34, (uint16_t *)n[j1]);
  M5.Lcd.pushImage( 99, 0,18,34, (uint16_t *)n[j2]);
  M5.Lcd.drawPixel(118,13, ORANGE); M5.Lcd.drawPixel(119,23,ORANGE);
  M5.Lcd.pushImage(120, 0,18,34, (uint16_t *)n[d1]);
  M5.Lcd.pushImage(140, 0,18,34, (uint16_t *)n[d2]);
                                                    
  M5.Lcd.pushImage( 00,40,18,34, (uint16_t *)n[h1]);
  M5.Lcd.pushImage( 20,40,18,34, (uint16_t *)n[h2]);
  M5.Lcd.drawPixel( 48,54, ORANGE); M5.Lcd.drawPixel( 48,64,ORANGE); 
  M5.Lcd.pushImage( 60,40,18,34, (uint16_t *)n[i1]);
  M5.Lcd.pushImage( 80,40,18,34, (uint16_t *)n[i2]);
  M5.Lcd.drawPixel(108,54, ORANGE); M5.Lcd.drawPixel(108,64,ORANGE);
  M5.Lcd.pushImage(120,40,18,34, (uint16_t *)n[s1]);
  M5.Lcd.pushImage(140,40,18,34, (uint16_t *)n[s2]);
 
  // if ( i1 == 0 && i2 == 0 ){ fade();}
} //vfd_2_line

void vfd_1_line(){
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Rtc.GetData(&RTC_DateStruct);
  int h1 = int(RTC_TimeStruct.Hours / 10 );
  int h2 = int(RTC_TimeStruct.Hours - h1*10 );
  int i1 = int(RTC_TimeStruct.Minutes / 10 );
  int i2 = int(RTC_TimeStruct.Minutes - i1*10 );
  int s0 = int(RTC_TimeStruct.Seconds);
  int s1 = (s0 * 2.7);                   // scale to screen width (60s = 160 pixel)
  
  if (0 == s0) M5.Lcd.fillScreen(BLACK);
  
  M5.Lcd.pushImage(  2,0,35,67, (uint16_t *)m[h1]);
  M5.Lcd.pushImage( 41,0,35,67, (uint16_t *)m[h2]);

  M5.Lcd.pushImage( 83,0,35,67, (uint16_t *)m[i1]);
  M5.Lcd.pushImage(121,0,35,67, (uint16_t *)m[i2]);

    if ((s0 % 2) == 0) {                                          // this will make the dots blink every second
      M5.Lcd.drawPixel( 79,28, BLACK); M5.Lcd.drawPixel( 79,54,BLACK);
      M5.Lcd.drawPixel( 79,27, BLACK); M5.Lcd.drawPixel( 79,53,BLACK);
      }
  else {
      M5.Lcd.drawPixel( 79,28, ORANGE); M5.Lcd.drawPixel( 79,54,ORANGE);
      M5.Lcd.drawPixel( 79,27, YELLOW); M5.Lcd.drawPixel( 79,53,YELLOW);
     }
  // Serial.print("seconds = "); Serial.print(s0); Serial.print("; bar = "); Serial.println(s1);
  M5.Lcd.drawPixel( s1,73, ORANGE); M5.Lcd.drawPixel( s1,74, YELLOW); // seconds bar
} //vfd_1_line

void vfd_X_line(){          // mm:ss
  M5.Rtc.GetTime(&RTC_TimeStruct);
  M5.Rtc.GetData(&RTC_DateStruct);
  int i1 = int(RTC_TimeStruct.Minutes / 10 );
  int i2 = int(RTC_TimeStruct.Minutes - i1*10 );
  int s1 = int(RTC_TimeStruct.Seconds / 10 );
  int s2 = int(RTC_TimeStruct.Seconds - s1*10 );
  
  M5.Lcd.pushImage(  2,6,35,67, (uint16_t *)m[i1]);
  M5.Lcd.pushImage( 41,6,35,67, (uint16_t *)m[i2]);
  M5.Lcd.drawPixel( 79,28, ORANGE); M5.Lcd.drawPixel( 79,54,ORANGE); 
  M5.Lcd.drawPixel( 79,27, YELLOW); M5.Lcd.drawPixel( 79,53,YELLOW); 
  M5.Lcd.pushImage( 83,6,35,67, (uint16_t *)m[s1]);
  M5.Lcd.pushImage(121,6,35,67, (uint16_t *)m[s2]);
 
  if ( s1 == 0 && s2 == 0 ){ fade();}
} //vfd_X_line

void fade(){
  for (int i=7;i<16;i++){M5.Axp.ScreenBreath(i);delay(25);}
  for (int i=15;i>7;i--){M5.Axp.ScreenBreath(i);delay(25);}
  M5.Axp.ScreenBreath(12);
} //fade
