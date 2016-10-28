
#include <SPI.h>
#include "Ucglib.h"
#include <Wire.h>  // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

RtcDS3231 Rtc;

Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 8 , /*cs=*/ 10, /*reset=*/ 9);

#define countof(a) (sizeof(a) / sizeof(a[0]))

byte  screen_x_max;
byte  screen_y_max;
byte  old_Hour   = 0;
byte  new_Hour;
byte  old_Minute = 0;
byte  new_Minute;
float old_temp   = 0.0;
float new_temp;
const unsigned int DELAY  = 1000;

void ucglib_graphics_test(void) {

  RtcDateTime now         = Rtc.GetDateTime();
  const RtcDateTime& dt   = now;
  RtcTemperature new_temp = Rtc.GetTemperature();
  new_Hour                = dt.Hour();
  new_Minute              = dt.Minute();

  char datestring[20];
  char timestring[20];

  snprintf_P(datestring, countof(datestring), PSTR("%02u.%02u.%04u"), dt.Day(), dt.Month(), dt.Year());
  snprintf_P(timestring, countof(timestring), PSTR("%02u:%02u"),      new_Hour, new_Minute);

  // Date
  ucg.setFont(ucg_font_logisoso16_tf);
  ucg.setColor(255, 168, 0);
  ucg.setPrintPos(9, 28);
  ucg.print(datestring);

  // Temperature
  ucg.setPrintPos(90, 15 + 100);
  ucg.print(new_temp.AsFloat());
  ucg.setPrintPos(142, 15 + 100);
  ucg.print("C");

  // Time
  ucg.setFont(ucg_font_logisoso50_tf);
  ucg.setColor(55, 208, 205);
  ucg.setPrintPos(7, 64 + 25);
  ucg.print(timestring);
}

void drawStuff(void) {
  ucg.setColor(128, 0, 255);
  ucg.drawFrame(3, 3, screen_y_max - 6, screen_x_max - 6);
  ucg.drawTriangle(120, 10, 130, 30, 150, 25);
}

void setup(void) {
  //rtc ----------
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid()) {
    Rtc.SetDateTime(compiled);
  }
  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }
  RtcDateTime now = Rtc.GetDateTime();

  if (now < compiled) {
    Rtc.SetDateTime(compiled);
  }

  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

  // ucglib ----------
  delay(DELAY);
  //ucg.powerDown();
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.clearScreen();
  //ucg.powerUp();

  screen_y_max = ucg.getHeight();
  screen_x_max = ucg.getWidth();
}

void loop(void) {
  delay(DELAY);
  if (old_Hour != new_Hour) {
    old_Hour = new_Hour;
    ucg.clearScreen();
  }
  if (old_Minute != new_Minute) {
    old_Minute = new_Minute;
    ucg.clearScreen();
  }
  if (old_temp != new_temp) {
    old_temp = new_temp;
    ucg.clearScreen();
  }

  ucg.setRotate90();
  ucglib_graphics_test();
  drawStuff();
}
