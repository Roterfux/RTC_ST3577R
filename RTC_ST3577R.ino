#include <SPI.h>
#include <Wire.h>  // required for RTC
#include "Ucglib.h"
#include <RtcDS3231.h>

// -------------------- RTC --------------------
RtcDS3231<TwoWire> Rtc(Wire);

// -------------------- Display --------------------
Ucglib_ST7735_18x128x160_SWSPI ucg(
  /*sclk=*/ 13,
  /*data=*/ 11,
  /*cd=*/   8,
  /*cs=*/   10,
  /*reset=*/9
);

// -------------------- Constants --------------------
constexpr uint16_t DELAY_MS = 1000;

template <typename T, size_t N>
constexpr size_t countof(T (&)[N]) { return N; }

// -------------------- State --------------------
uint8_t screen_x_max = 0;
uint8_t screen_y_max = 0;

uint8_t old_Hour   = 0;
uint8_t new_Hour   = 0;
uint8_t old_Minute = 0;
uint8_t new_Minute = 0;

float old_temp = 0.0f;
float new_temp = 0.0f;

// -------------------- UI Rendering --------------------
void drawClock()
{
  const RtcDateTime now = Rtc.GetDateTime();
  const RtcTemperature temp = Rtc.GetTemperature();

  new_Hour   = now.Hour();
  new_Minute = now.Minute();
  new_temp   = temp.AsFloat();

  // Adjust hour (kept original behavior)
  if (new_Hour == 0)
    new_Hour = 23;
  else if (new_Hour > 23)
    new_Hour = 0;

  char datestring[20];
  char timestring[20];

  snprintf_P(datestring, countof(datestring),
             PSTR("%02u.%02u.%04u"),
             now.Day(), now.Month(), now.Year());

  snprintf_P(timestring, countof(timestring),
             PSTR("%02u:%02u"),
             static_cast<uint8_t>(new_Hour - 1),
             new_Minute);

  // Date
  ucg.setFont(ucg_font_logisoso16_tf);
  ucg.setColor(255, 168, 0);
  ucg.setPrintPos(9, 28);
  ucg.print(datestring);

  // Temperature
  ucg.setPrintPos(90, 115);
  ucg.print(new_temp);
  ucg.setPrintPos(142, 115);
  ucg.print("C");

  // Time
  ucg.setFont(ucg_font_logisoso50_tf);
  ucg.setColor(55, 208, 205);
  ucg.setPrintPos(7, 89);
  ucg.print(timestring);
}

void drawDecorations()
{
  ucg.setColor(128, 50, 255);
  ucg.drawFrame(3, 3, screen_y_max - 6, screen_x_max - 6);

  ucg.drawTriangle(
    120, 10,
    130, 30,
    150, 25
  );
}

// -------------------- Setup --------------------
void setup()
{
  // ----- RTC -----
  Rtc.Begin();

  const RtcDateTime compiled(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid())
    Rtc.SetDateTime(compiled);

  if (!Rtc.GetIsRunning())
    Rtc.SetIsRunning(true);

  if (Rtc.GetDateTime() < compiled)
    Rtc.SetDateTime(compiled);

  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

  // ----- Display -----
  delay(DELAY_MS);

  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.clearScreen();

  screen_y_max = ucg.getHeight();
  screen_x_max = ucg.getWidth();
}

// -------------------- Loop --------------------
void loop()
{
  delay(DELAY_MS);

  // Only clear screen if something changed
  if (old_Hour != new_Hour ||
      old_Minute != new_Minute ||
      fabs(old_temp - new_temp) > 0.1f)  // safer float compare
  {
    old_Hour   = new_Hour;
    old_Minute = new_Minute;
    old_temp   = new_temp;

    ucg.clearScreen();
  }

  ucg.setRotate90();

  drawClock();
  drawDecorations();
}
