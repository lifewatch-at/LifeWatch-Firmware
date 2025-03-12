#include "myDisplay.h"

#ifdef DISPLAY_750
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> 
#ifdef ARDUINO_ESP32S2_DEV
display(GxEPD2_750_T7(10, 14, 21, 33)); //CS, DC, RST, BUSY - ESP32-S2
#else
display(GxEPD2_750_T7(5, 27, 26, 25)); //CS, DC, RST, BUSY - ESP32
#endif
#define CORRECTION 1
#define COR        1
#endif //DISPLAY_750

#ifndef DISPLAY_750
GxEPD2_BW <GxEPD2_1330_GDEM133T91, GxEPD2_1330_GDEM133T91::HEIGHT / 2>
#ifdef ARDUINO_ESP32S2_DEV
display(GxEPD2_1330_GDEM133T91(10, 14, 21, 33)); // ESP32-S2
#else
display(GxEPD2_1330_GDEM133T91(5, 27, 26, 25)); // ESP32
#endif
#define CORRECTION 0.84705882+52
#define COR        0.84705882
#endif //DISPLAY_750

#ifdef ARDUINO_ESP32S2_DEV
SPIClass spi(HSPI);
#endif

void myDisplay::init()
{
#ifdef ARDUINO_ESP32S2_DEV
  spi.begin(12, -1, 11, 5);
  display.init(115200, true, 10, false, spi, SPISettings(500000, MSBFIRST, SPI_MODE0));
  pinMode(5, OUTPUT);
#endif

  display.init(115200, true, 2, false);

  display.setRotation(0);
  display.setFont(&bold_95);
  display.setTextColor(GxEPD_BLACK);

  /***   Time   ***/
  int16_t tbx_time, tby_time; uint16_t tbw_time, tbh_time;
  display.getTextBounds(mhTime, 0, 0, &tbx_time, &tby_time, &tbw_time, &tbh_time);
  uint16_t x_time = ((display.width()  - tbw_time) / 2) - tbx_time;
  uint16_t y_time = ((display.height()*CORRECTION - tbh_time) / 2) - tby_time - (display.height()*0.1); //0.0458

  /***   Temp   ***/
  display.setFont(&FreeMonoBold24pt7b);
  int16_t tbx_temp, tby_temp; uint16_t tbw_temp, tbh_temp;
  display.getTextBounds(_cTemp, 0, 0, &tbx_temp, &tby_temp, &tbw_temp, &tbh_temp);
  int x_temp = ((display.width()* 0.4  - tbw_temp) / 2) - tbx_temp + display.width()*0.6;
  int y_temp = ((display.height()*0.25*CORRECTION - tbh_temp) / 2) - tby_temp;

  /***   Hum   ***/
  int16_t tbx_hum, tby_hum; uint16_t tbw_hum, tbh_hum;
  display.getTextBounds(_cHum, 0, 0, &tbx_hum, &tby_hum, &tbw_hum, &tbh_hum);
  int x_hum = ((display.width()* 0.4  - tbw_hum) / 2) - tbx_hum;
  int y_hum = ((display.height()*0.25*CORRECTION - tbh_hum) / 2) - tby_hum;

  /***   CO2   ***/
  int16_t tbx2_co2, tby2_co2; uint16_t tbw2_co2, tbh2_co2;
  display.getTextBounds(CO2, 0, 0, &tbx2_co2, &tby2_co2, &tbw2_co2, &tbh2_co2);

  /***   TVOC   ***/
  int16_t tbx2_tvoc, tby2_tvoc; uint16_t tbw2_tvoc, tbh2_tvoc;
  display.getTextBounds(TVOC, 0, 0, &tbx2_tvoc, &tby2_tvoc, &tbw2_tvoc, &tbh2_tvoc);

  /***   CO   ***/
  int16_t tbx2_co, tby2_co; uint16_t tbw2_co, tbh2_co;
  display.getTextBounds(CO, 0, 0, &tbx2_co, &tby2_co, &tbw2_co, &tbh2_co);

  display.setFullWindow();
  display.firstPage();

  do
  {
    display.fillScreen(GxEPD_WHITE);

    /***   print Time   ***/
    display.setFont(&bold_95);
    display.setCursor(x_time, y_time);
    display.print(mhTime);

    /***   print Temp   ***/
    display.setFont(&FreeMonoBold24pt7b);
    display.setCursor(x_temp, y_temp);
    display.print(_cTemp);

    /***   print Hum   ***/
    display.setCursor(x_hum, y_hum);
    display.print(_cHum);

    /***   Balken für CO2   ***/
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

    /***   print CO2 text    ***/
    display.setCursor(x + w - tbw2_co2 - 15, y_3 + tbh2_co2 + 8);
    display.print(CO2);

    /***   print TVOC text   ***/
    display.setCursor(x + w - tbw2_tvoc - 15, y_2 + tbh2_tvoc + 8);
    display.print(TVOC);

    /***   print CO   text   ***/
    display.setCursor(x + w - tbw2_co - 15, y_1 + tbh2_co + 8);
    display.print(CO);

    display.drawXBitmap(0,                   display.height()*0.67708*CORRECTION, happy_smiley_bits, SMILEY_WIDTH, SMILEY_HEIGHT, GxEPD_BLACK);
    display.drawXBitmap(display.width()-100, display.height()*0.67708*CORRECTION, sad_smiley_bits,   SMILEY_WIDTH, SMILEY_HEIGHT, GxEPD_BLACK);
  }
  while (display.nextPage());

  // delay(1000);

  // updateSpace_1("hum:34.6%");
  // updateSpace_2("temp:23.8C");
  // updateSpace_3("CO2",   190,  100,  250);
  // updateSpace_4("CO",     13,   10,   20);
  // updateSpace_5("TVOC", 1445, 1000, 2000);
  // updateTime(11, 33, true);

  // BatteryWarning(true);
  // delay(5000);
  // BatteryWarning(false);
  // delay(1000);
  // display.refresh();

  display.hibernate();
}

void myDisplay::deepSleep() 
{
  display.hibernate();
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

  display.setPartialWindow(display.width()*0.3125, display.height()*0.25*CORRECTION, 
                           display.width()*0.375 , display.height()*0.25*COR);

  display.setRotation(0);
  display.setFont(&bold_95);
  display.setTextColor(GxEPD_BLACK);

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