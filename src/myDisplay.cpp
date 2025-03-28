#include "myDisplay.h"

#ifdef DISPLAY_750
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> 
#ifdef ARDUINO_ESP32S2_DEV
display(GxEPD2_750_T7(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)); // ESP32-S2
#else
display(GxEPD2_750_T7(5, 27, 26, 25)); //CS, DC, RST, BUSY - ESP32
#endif
#define CORRECTION 1
#define COR        1
#endif //DISPLAY_750

#ifndef DISPLAY_750
GxEPD2_BW <GxEPD2_1330_GDEM133T91, GxEPD2_1330_GDEM133T91::HEIGHT / 2>
#ifdef ARDUINO_ESP32S2_DEV
display(GxEPD2_1330_GDEM133T91(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)); // ESP32-S2
#else
display(GxEPD2_1330_GDEM133T91(5, 27, 26, 25)); // ESP32
#endif
#define CORRECTION 0.84705882+52
#define COR        0.84705882
#endif //DISPLAY_750

#ifdef ARDUINO_ESP32S2_DEV
SPIClass spi(HSPI);
#endif

void myDisplay::init(bool initial) {
#ifdef ARDUINO_ESP32S2_DEV
  spi.begin(EPD_SCK, -1, EPD_DIN, EPD_CS);
  display.init(115200, initial, 10, false, spi, SPISettings(1e7, MSBFIRST, SPI_MODE0));
#else
  display.init(115200, initial, 2, false);
#endif
}

void myDisplay::updateTextbox(const char *space_1, const char *space_2, bool show_battery_warning, bool show_WiFi, const char *display_ssid) {
  
  display.setPartialWindow(0, 0, display.width(), display.height() * 0.2);
  display.setFont(&FreeMonoBold24pt7b);
  
  if (show_battery_warning && show_WiFi) {
    do {
      display.fillScreen(GxEPD_WHITE);
      display.setCursor(display.width() * 0.14, display.height() * 0.14);
      display.print(display_ssid);

      display.drawXBitmap(display.width() * 0.85, display.height() * 0.06, Akkuwarnung, AKKUWARNUNG_WIDTH, AKKUWARNUNG_HEIGHT, GxEPD_BLACK);
      display.drawXBitmap(display.width() * 0.02, 0, WiFi_symbol, WIFI_SYMBOL_WIDTH, WIFI_SYMBOL_HEIGHT, GxEPD_BLACK);
    } while (display.nextPage());
  }
  else if (show_WiFi) {
    do {
      int x = display.width() * 0.123;
      int16_t tbx, tby; uint16_t tbw, tbh;
      int y;

      display.fillScreen(GxEPD_WHITE);

      display.drawXBitmap(display.width() * 0.02, 0, WiFi_symbol, WIFI_SYMBOL_WIDTH, WIFI_SYMBOL_HEIGHT, GxEPD_BLACK);

      display.setCursor(display.width() * 0.14, display.height() * 0.14);
      display.print(display_ssid);
    } while (display.nextPage());
  }
  else if (show_battery_warning) {
    do {
      int x = display.width() * 0.123;
      int16_t tbx, tby; uint16_t tbw, tbh;
      int y;
  
      display.fillScreen(GxEPD_WHITE);

      display.drawXBitmap(display.width() * 0.85, display.height() * 0.06, Akkuwarnung, AKKUWARNUNG_WIDTH, AKKUWARNUNG_HEIGHT, GxEPD_BLACK);
  
      /*Space_1*/
      display.getTextBounds(space_1, 0, 0, &tbx, &tby, &tbw, &tbh);
      x = ((display.width() *0.4  - tbw) / 2) - tbx;
      y = ((display.height()*0.25*CORRECTION - tbh) / 2) - tby;
      display.setCursor(x, display.height() * 0.14);
      display.print(space_1);
    } while (display.nextPage());
  }
  else {
    do {
      int x = display.width() * 0.123;
      int16_t tbx, tby; uint16_t tbw, tbh;
      int y;

      display.fillScreen(GxEPD_WHITE);
  
      /*Space_1*/
      display.getTextBounds(space_1, 0, 0, &tbx, &tby, &tbw, &tbh);
      x = ((display.width() *0.4  - tbw) / 2) - tbx;
      y = ((display.height()*0.25*CORRECTION - tbh) / 2) - tby;
      display.setCursor(x, display.height() * 0.14);
      display.print(space_1);
  
      /*Space_2*/
      display.getTextBounds(space_2, 0, 0, &tbx, &tby, &tbw, &tbh);
      x = ((display.width() *0.4 - tbw) / 2) - tbx + display.width()*0.6;
      y = ((display.height()*0.25*CORRECTION - tbh) / 2) - tby;
      display.setCursor(x, display.height() * 0.14);
      display.print(space_2);
    } while (display.nextPage());
  }
}

void myDisplay::updateBars(
  const char *name_space_3, const float value_space_3, const float min_space_3, const float max_space_3,
  const char *name_space_4, const float value_space_4, const float min_space_4, const float max_space_4,
  const char *name_space_5, const float value_space_5, const float min_space_5, const float max_space_5)
{
  display.setPartialWindow(0, display.height() * 0.55, display.width(), display.height() * 0.45);
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);


  do {
    display.fillScreen(GxEPD_WHITE);

    /*Balken*/
    int x   = display.width() * 0.123; 
    int w   = display.width() * 0.7563;
    int h   = display.height()* 0.1042 * COR;
    int y_1 = display.height()* 0.6042 * CORRECTION;
    int y_2 = display.height()* 0.7292 * CORRECTION;
    int y_3 = display.height()* 0.8542 * CORRECTION;

    display.fillRect(x    , y_1    , w     , h     , GxEPD_BLACK);
    display.fillRect(x + 5, y_1 + 5, w - 10, h - 10, GxEPD_WHITE);

    display.fillRect(x    , y_2    , w     , h     , GxEPD_BLACK);
    display.fillRect(x + 5, y_2 + 5, w - 10, h - 10, GxEPD_WHITE);

    display.fillRect(x    , y_3    , w     , h     , GxEPD_BLACK);
    display.fillRect(x + 5, y_3 + 5, w - 10, h - 10, GxEPD_WHITE);

    /*Smileys*/
    display.drawXBitmap(0,                   display.height()*0.67708*CORRECTION, happy_smiley_bits, SMILEY_WIDTH, SMILEY_HEIGHT, GxEPD_BLACK);
    display.drawXBitmap(display.width()-100, display.height()*0.67708*CORRECTION, sad_smiley_bits,   SMILEY_WIDTH, SMILEY_HEIGHT, GxEPD_BLACK);

    /*common Space 1 - 5*/
    int16_t tbx, tby; uint16_t tbw, tbh;
    int y;

    display.setFont(&FreeMonoBold24pt7b);

    /*common Space 3 - 5*/
    uint16_t new_w;
    bool unter_haelfte;

    /*Space_3*/
    unter_haelfte = !((((max_space_3 - min_space_3) / 2) + min_space_3) > value_space_3); 
    new_w = map(value_space_3, min_space_3, max_space_3, 0, display.width()*0.7563-20 /*display.width()*0.725*/);
    new_w = constrain(new_w, 0, (display.width()*0.7563)-20);

    display.getTextBounds(name_space_3, 0, 0, &tbx, &tby, &tbw, &tbh);

    display.fillRect(display.width()*0.1375, display.height()*0.61875*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.61875*CORRECTION + tbh);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name_space_3);

    /*Space_4*/
    unter_haelfte = !((((max_space_4 - min_space_4) / 2) + min_space_4) > value_space_4); 
    new_w = map(value_space_4, min_space_4, max_space_4, 0, display.width()*0.7563-20 /*display.width()*0.725*/);
    new_w = constrain(new_w, 0, (display.width()*0.7563)-20);
    display.getTextBounds(name_space_4, 0, 0, &tbx, &tby, &tbw, &tbh);

    display.fillRect(display.width()*0.1375, display.height()*0.74375*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.74375*CORRECTION + tbh+2);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name_space_4);

    /*Space_5*/
    unter_haelfte = !((((max_space_5 - min_space_5) / 2) + min_space_5) > value_space_5); 
    new_w = map(value_space_5, min_space_5, max_space_5, 0, display.width()*0.7563-20 /*display.width()*0.725*/);
    new_w = constrain(new_w, 0, (display.width()*0.7563)-20);
    display.getTextBounds(name_space_5, 0, 0, &tbx, &tby, &tbw, &tbh);

    display.fillRect(display.width()*0.1375, display.height()*0.86875*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.86875*CORRECTION + tbh+2);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name_space_5);

  } while (display.nextPage());
}

void myDisplay::updateEverything(int hour, int min, const char* space_1, const char* space_2,
const char *name_space_3, const float value_space_3, const float min_space_3, const float max_space_3,
const char *name_space_4, const float value_space_4, const float min_space_4, const float max_space_4,
const char *name_space_5, const float value_space_5, const float min_space_5, const float max_space_5)
{
  display.setFullWindow();
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
  char time[6];

  if (hour < 10 && min >= 10) {
    sprintf(time, "0%d:%d", hour, min);
  }
  else if (hour >= 10 && min < 10) {
    sprintf(time, "%d:0%d", hour, min);
  }
  else if (hour < 10 && min < 10) {
    sprintf(time, "0%d:0%d", hour, min);
  }
  else {
    sprintf(time, "%d:%d", hour, min);
  }

  do {
    display.fillScreen(GxEPD_WHITE);

    /*Balken*/
    int x   = display.width() * 0.123; 
    int w   = display.width() * 0.7563;
    int h   = display.height()* 0.1042 * COR;
    int y_1 = display.height()* 0.6042 * CORRECTION;
    int y_2 = display.height()* 0.7292 * CORRECTION;
    int y_3 = display.height()* 0.8542 * CORRECTION;

    display.fillRect(x    , y_1    , w     , h     , GxEPD_BLACK);
    display.fillRect(x + 5, y_1 + 5, w - 10, h - 10, GxEPD_WHITE);

    display.fillRect(x    , y_2    , w     , h     , GxEPD_BLACK);
    display.fillRect(x + 5, y_2 + 5, w - 10, h - 10, GxEPD_WHITE);

    display.fillRect(x    , y_3    , w     , h     , GxEPD_BLACK);
    display.fillRect(x + 5, y_3 + 5, w - 10, h - 10, GxEPD_WHITE);

    /*Smileys*/
    display.drawXBitmap(0,                   display.height()*0.67708*CORRECTION, happy_smiley_bits, SMILEY_WIDTH, SMILEY_HEIGHT, GxEPD_BLACK);
    display.drawXBitmap(display.width()-100, display.height()*0.67708*CORRECTION, sad_smiley_bits,   SMILEY_WIDTH, SMILEY_HEIGHT, GxEPD_BLACK);

    /*common Space 1 - 5*/
    display.setFont(&bold_95);
    int16_t tbx, tby; uint16_t tbw, tbh;
    int y;

    /*time*/
    display.getTextBounds(time, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width()  - tbw) / 2) - tbx;
    y = ((display.height()*CORRECTION - tbh) / 2) - tby - (display.height()*0.1);

    display.setCursor(x, y);
    display.print(time);

    display.setFont(&FreeMonoBold24pt7b);

    /*Space_1*/
    display.getTextBounds(space_1, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() *0.4  - tbw) / 2) - tbx;
    y = ((display.height()*0.25*CORRECTION - tbh) / 2) - tby;
    display.setCursor(x, y);
    display.print(space_1);

    /*Space_2*/
    display.getTextBounds(space_2, 0, 0, &tbx, &tby, &tbw, &tbh);
    x = ((display.width() *0.4 - tbw) / 2) - tbx + display.width()*0.6;
    y = ((display.height()*0.25*CORRECTION - tbh) / 2) - tby;
    display.setCursor(x, y);
    display.print(space_2);

    /*common Space 3 - 5*/
    uint16_t new_w;
    bool unter_haelfte;

    /*Space_3*/
    unter_haelfte = !((((max_space_3 - min_space_3) / 2) + min_space_3) > value_space_3); 
    new_w = map(value_space_3, min_space_3, max_space_3, 0, display.width()*0.7563-20 /*display.width()*0.725*/);
    new_w = constrain(new_w, 0, (display.width()*0.7563)-20);

    display.getTextBounds(name_space_3, 0, 0, &tbx, &tby, &tbw, &tbh);

    display.fillRect(display.width()*0.1375, display.height()*0.61875*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.61875*CORRECTION + tbh);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name_space_3);

    /*Space_4*/
    unter_haelfte = !((((max_space_4 - min_space_4) / 2) + min_space_4) > value_space_4); 
    new_w = map(value_space_4, min_space_4, max_space_4, 0, display.width()*0.7563-20 /*display.width()*0.725*/);
    new_w = constrain(new_w, 0, (display.width()*0.7563)-20);
    display.getTextBounds(name_space_4, 0, 0, &tbx, &tby, &tbw, &tbh);

    display.fillRect(display.width()*0.1375, display.height()*0.74375*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.74375*CORRECTION + tbh+2);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name_space_4);

    /*Space_5*/
    unter_haelfte = !((((max_space_5 - min_space_5) / 2) + min_space_5) > value_space_5); 
    new_w = map(value_space_5, min_space_5, max_space_5, 0, display.width()*0.7563-20 /*display.width()*0.725*/);
    new_w = constrain(new_w, 0, (display.width()*0.7563)-20);
    display.getTextBounds(name_space_5, 0, 0, &tbx, &tby, &tbw, &tbh);

    display.fillRect(display.width()*0.1375, display.height()*0.86875*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.86875*CORRECTION + tbh+2);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name_space_5);



  } while (display.nextPage());
}

void myDisplay::powerOff() 
{
  display.powerOff();
}

void myDisplay::printBox(bool isBlack, uint16_t x, uint16_t y, uint16_t w, 
                         uint16_t h, bool partial) {
  display.setRotation(0);

  if (partial) { display.setPartialWindow(x, y, w - 3, h); }
  else { display.setFullWindow(); }

  display.firstPage();
  do
  {
    display.fillScreen(isBlack ? GxEPD_WHITE : GxEPD_BLACK);
    display.fillRect(x, y, w, h, isBlack ? GxEPD_BLACK : GxEPD_WHITE);
  }
  while (display.nextPage());
}

uint16_t myDisplay::map(uint16_t x, uint16_t IN_min, uint16_t IN_max, 
                        uint16_t OUT_min, uint16_t OUT_max)
{ return ((((x - IN_min)*(OUT_max - OUT_min))/(IN_max - IN_min)) + OUT_min); }

void myDisplay::updateSpace_1(const char *val_space_1) {
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold24pt7b);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(val_space_1, 0, 0, &tbx, &tby, &tbw, &tbh);
  int x = ((display.width() *0.4  - tbw) / 2) - tbx;
  int y = ((display.height()*0.25*CORRECTION - tbh) / 2) - tby;

  display.setPartialWindow(0, 0*CORRECTION, display.width()*0.4, display.height()*0.25*COR);

  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(val_space_1);
  }
  while (display.nextPage());
}

void myDisplay::updateSpace_2(const char *val_space_2) {
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold24pt7b);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(val_space_2, 0, 0, &tbx, &tby, &tbw, &tbh);
  int x = ((display.width() *0.4 - tbw) / 2) - tbx + display.width()*0.6;
  int y = ((display.height()*0.25*CORRECTION - tbh) / 2) - tby;

  display.setPartialWindow(display.width()*0.6, 0*CORRECTION, display.width()*0.4, display.height()*0.25*COR);

  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(val_space_2);
  }
  while (display.nextPage());
}

void myDisplay::updateSpace_3(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, display.width()*0.725);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display.setRotation(0);
  display.setPartialWindow(display.width()*0.123+10,  display.height()*0.6042*CORRECTION+5, 
                           display.width()*0.7563-20, display.height()*0.1042*COR-10); // x, y, w, h

  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(display.width()*0.1375, display.height()*0.61875*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.61875*CORRECTION + tbh);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name);
  }
  while (display.nextPage());
}

void myDisplay::updateSpace_4(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, display.width()*0.725);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display.setRotation(0);
  display.setPartialWindow(display.width()*0.123+10,  display.height()*0.7292*CORRECTION+5, 
                           display.width()*0.7563-20, display.height()*0.1042*COR-10); // x, y, w, h

  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(display.width()*0.1375, display.height()*0.74375*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.74375*CORRECTION + tbh+2);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name);
  }
  while (display.nextPage());
}

void myDisplay::updateSpace_5(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, display.width()*0.725);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display.setRotation(0);
  display.setPartialWindow(display.width()*0.123+10,  display.height()*0.8542*CORRECTION+5, 
                           display.width()*0.7563-20, display.height()*0.1042*COR-10); // x, y, w, h

  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(display.width()*0.1375, display.height()*0.86875*CORRECTION, new_w, display.height()*0.075*COR, GxEPD_BLACK);
    display.setCursor(display.width()*0.14625 + (unter_haelfte ? 0 : display.width()*0.725 - tbw - 4), display.height()*0.86875*CORRECTION + tbh+2);
    display.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display.print(name);
  }
  while (display.nextPage());
}

void myDisplay::updateTime(int hour, int min, bool partial) {
  char time[6];

  if (hour < 10 && min >= 10) {
    sprintf(time, "0%d:%d", hour, min);
  }
  else if (hour >= 10 && min < 10) {
    sprintf(time, "%d:0%d", hour, min);
  }
  else if (hour < 10 && min < 10) {
    sprintf(time, "0%d:0%d", hour, min);
  }
  else {
    sprintf(time, "%d:%d", hour, min);
  }

#ifdef DISPLAY_750
  display.setPartialWindow(display.width()*0.1, display.height()*0.25*COR /*CORRECTION*/, 
                           display.width()*0.8 , display.height()*0.3*COR);
#else
  display.setPartialWindow(0, display.height()*0.2*COR, 
                           display.width(), display.height()*0.45*COR);
#endif //DISPLAY_750

  display.setRotation(0);
  display.setFont(&bold_95);
  display.setTextColor(GxEPD_BLACK);

#ifdef DISPLAY_750
  display.setTextSize(2);
#else
  display.setTextSize(3);
#endif //DISPLAY_750

  int16_t tbx_time, tby_time; uint16_t tbw_time, tbh_time;
  display.getTextBounds(time, 0, 0, &tbx_time, &tby_time, &tbw_time, &tbh_time);
  uint16_t x_time = ((display.width()  - tbw_time) / 2) - tbx_time;
  uint16_t y_time = ((display.height()*CORRECTION - tbh_time) / 2) - tby_time - (display.height()*0.1);

  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x_time, y_time);
    display.print(time);
  } 
  while (display.nextPage());

  display.setTextSize(1);
  
  if (!partial) {display.refresh();}
}

void myDisplay::refresh() {
  display.refresh();
}

void myDisplay::BatteryWarning(bool show_warning) {
  if (show_warning) {
    display.setPartialWindow(0,display.height()*0.3, display.width()*0.15, display.height()*0.2);
    do
    {
      display.fillScreen(GxEPD_WHITE);
      display.drawXBitmap(0, display.height()*0.36, Akkuwarnung, AKKUWARNUNG_WIDTH, AKKUWARNUNG_HEIGHT, GxEPD_BLACK);
    } while (display.nextPage());
  }
  else {
    display.setPartialWindow(0,display.height()*0.3, display.width()*0.15, display.height()*0.2);
    do
    {
      display.fillScreen(GxEPD_WHITE);
    } while (display.nextPage());
    
  }
}