#include "myDisplay.h"

#ifndef DISPLAY_750

GxEPD2_BW <GxEPD2_1330_GDEM133T91, GxEPD2_1330_GDEM133T91::HEIGHT / 2> 
display_1330(GxEPD2_1330_GDEM133T91(5, 27, 26, 25)); //CS, DC, RST, BUSY


void myDisplay_1330::init()
{
  display_1330.init(115200, true, 2, false);

  display_1330.setRotation(0);
  display_1330.setFont(&bold_95);
  display_1330.setTextColor(GxEPD_BLACK);

  /***   Time   ***/
  int16_t tbx_time, tby_time; uint16_t tbw_time, tbh_time;
  display_1330.getTextBounds(mhTime, 0, 0, &tbx_time, &tby_time, &tbw_time, &tbh_time);
  uint16_t x_time = ((display_1330.width() - tbw_time) / 2) - tbx_time;
  uint16_t y_time = ((display_1330.height() - tbh_time) / 2) - tby_time - 70;

  /***   Temp   ***/
  display_1330.setFont(&FreeMonoBold24pt7b);
  int16_t tbx_temp, tby_temp; uint16_t tbw_temp, tbh_temp;
  display_1330.getTextBounds(_cTemp, 0, 0, &tbx_temp, &tby_temp, &tbw_temp, &tbh_temp);
  int x_temp = ((320 - tbw_temp) / 2) - tbx_temp + 480;
  int y_temp = ((120 - tbh_temp) / 2) - tby_temp;

  /***   Hum   ***/
  int16_t tbx_hum, tby_hum; uint16_t tbw_hum, tbh_hum;
  display_1330.getTextBounds(_cHum, 0, 0, &tbx_hum, &tby_hum, &tbw_hum, &tbh_hum);
  int x_hum = ((320 - tbw_hum) / 2) - tbx_hum;
  int y_hum = ((120 - tbh_hum) / 2) - tby_hum;

  /***   CO2   ***/
  int16_t tbx2_co2, tby2_co2; uint16_t tbw2_co2, tbh2_co2;
  display_1330.getTextBounds(CO2, 0, 0, &tbx2_co2, &tby2_co2, &tbw2_co2, &tbh2_co2);

  /***   TVOC   ***/
  int16_t tbx2_tvoc, tby2_tvoc; uint16_t tbw2_tvoc, tbh2_tvoc;
  display_1330.getTextBounds(TVOC, 0, 0, &tbx2_tvoc, &tby2_tvoc, &tbw2_tvoc, &tbh2_tvoc);

  /***   CO   ***/
  int16_t tbx2_co, tby2_co; uint16_t tbw2_co, tbh2_co;
  display_1330.getTextBounds(CO, 0, 0, &tbx2_co, &tby2_co, &tbw2_co, &tbh2_co);

  display_1330.setFullWindow();
  display_1330.firstPage();

  do
  {
    display_1330.fillScreen(GxEPD_WHITE);

    /***   print Time   ***/
    display_1330.setFont(&bold_95);
    display_1330.setCursor(x_time, y_time);
    display_1330.print(mhTime);

    /***   print Temp   ***/
    display_1330.setFont(&FreeMonoBold24pt7b);
    display_1330.setCursor(x_temp, y_temp);
    display_1330.print(_cTemp);

    /***   print Hum   ***/
    display_1330.setCursor(x_hum, y_hum);
    display_1330.print(_cHum);

    /***   Balken für CO2   ***/
    int x   = 100;
    int w   = 605;
    int h   =  50;
    int y_1 = 290;
    int y_2 = 350;
    int y_3 = 410;

    display_1330.fillRect(x    , y_1    , w     , h     , GxEPD_BLACK);
    display_1330.fillRect(x + 5, y_1 + 5, w - 10, h - 10, GxEPD_WHITE);

    display_1330.fillRect(x    , y_2    , w     , h     , GxEPD_BLACK);
    display_1330.fillRect(x + 5, y_2 + 5, w - 10, h - 10, GxEPD_WHITE);

    display_1330.fillRect(x    , y_3    , w     , h     , GxEPD_BLACK);
    display_1330.fillRect(x + 5, y_3 + 5, w - 10, h - 10, GxEPD_WHITE);

    /***   print CO2 text    ***/
    display_1330.setCursor(x + w - tbw2_co2 - 15, y_3 + tbh2_co2 + 8);
    display_1330.print(CO2);

    /***   print TVOC text   ***/
    display_1330.setCursor(x + w - tbw2_tvoc - 15, y_2 + tbh2_tvoc + 8);
    display_1330.print(TVOC);

    /***   print CO   text   ***/
    display_1330.setCursor(x + w - tbw2_co - 15, y_1 + tbh2_co + 8);
    display_1330.print(CO);

    display_1330.drawXBitmap(0, 325, happy_smiley_bits, 100, 100, GxEPD_BLACK);
    display_1330.drawXBitmap(702, 325, sad_smiley_bits, 100, 100, GxEPD_BLACK);
  }
  while (display_1330.nextPage());

  // delay(1000);
  // updateSpace_1("hum:34.6%");
  // updateSpace_2("temp:23.8C");
  // updateSpace_3("CO2",   220,  100,  250);
  // updateSpace_4("CO",     13,   10,   20);
  // updateSpace_5("TVOC", 1445, 1000, 2000);
  // update_time(11, 33, false);

  display_1330.hibernate();
}

void myDisplay_1330::deepSleep() 
{
  display_1330.hibernate();
}

void myDisplay_1330::printBox(bool isBlack, uint16_t x, uint16_t y, uint16_t w, 
                         uint16_t h, bool partial) {
  display_1330.setRotation(0);

  if (partial) { display_1330.setPartialWindow(x, y, w - 3, h); }
  else { display_1330.setFullWindow(); }

  display_1330.firstPage();
  do
  {
    display_1330.fillScreen(isBlack ? GxEPD_WHITE : GxEPD_BLACK);
    display_1330.fillRect(x, y, w, h, isBlack ? GxEPD_BLACK : GxEPD_WHITE);
  }
  while (display_1330.nextPage());
}

uint16_t myDisplay_1330::map(uint16_t x, uint16_t IN_min, uint16_t IN_max, 
                        uint16_t OUT_min, uint16_t OUT_max)
{ return ((((x - IN_min)*(OUT_max - OUT_min))/(IN_max - IN_min)) + OUT_min); }

void myDisplay_1330::updateSpace_1(const char *val_space_1) {
  display_1330.setRotation(0);
  display_1330.setTextColor(GxEPD_BLACK);
  display_1330.setFont(&FreeMonoBold24pt7b);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display_1330.getTextBounds(val_space_1, 0, 0, &tbx, &tby, &tbw, &tbh);
  int x = ((320 - tbw) / 2) - tbx;
  int y = ((120 - tbh) / 2) - tby;

  display_1330.setPartialWindow(0, 0, 320, 120);

  do
  {
    display_1330.fillScreen(GxEPD_WHITE);
    display_1330.setCursor(x, y);
    display_1330.print(val_space_1);
  }
  while (display_1330.nextPage());
}

void myDisplay_1330::updateSpace_2(const char *val_space_2) {
  display_1330.setRotation(0);
  display_1330.setTextColor(GxEPD_BLACK);
  display_1330.setFont(&FreeMonoBold24pt7b);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display_1330.getTextBounds(val_space_2, 0, 0, &tbx, &tby, &tbw, &tbh);
  int x = ((320 - tbw) / 2) - tbx + 480;
  int y = ((120 - tbh) / 2) - tby;

  display_1330.setPartialWindow(480, 0, 320, 120);

  do
  {
    display_1330.fillScreen(GxEPD_WHITE);
    display_1330.setCursor(x, y);
    display_1330.print(val_space_2);
  }
  while (display_1330.nextPage());
}

void myDisplay_1330::updateSpace_3(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, 580);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display_1330.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display_1330.setRotation(0);
  display_1330.setPartialWindow(115, 297, 580, 36); // x, y, w, h

  display_1330.firstPage();
  do
  {
    display_1330.fillScreen(GxEPD_WHITE);
    display_1330.fillRect(110, 297, new_w, 36, GxEPD_BLACK);
    display_1330.setCursor(117 + (unter_haelfte ? 0 : 580 - tbw - 4), 297 + tbh);
    display_1330.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display_1330.print(name);
  }
  while (display_1330.nextPage());
}

void myDisplay_1330::updateSpace_4(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, 580);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display_1330.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display_1330.setRotation(0);
  display_1330.setPartialWindow(115, 357, 580, 36); // x, y, w, h

  display_1330.firstPage();
  do
  {
    display_1330.fillScreen(GxEPD_WHITE);
    display_1330.fillRect(110, 357, new_w, 36, GxEPD_BLACK);
    display_1330.setCursor(117 + (unter_haelfte ? 0 : 580 - tbw - 4), 357 + tbh + 2);
    display_1330.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display_1330.print(name);
  }
  while (display_1330.nextPage());
}

void myDisplay_1330::updateSpace_5(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, 580);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display_1330.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display_1330.setRotation(0);
  display_1330.setPartialWindow(115, 417, 580, 36); // x, y, w, h

  display_1330.firstPage();
  do
  {
    display_1330.fillScreen(GxEPD_WHITE);
    display_1330.fillRect(110, 417, new_w, 36, GxEPD_BLACK);
    display_1330.setCursor(117 + (unter_haelfte ? 0 : 580 - tbw - 4), 417 + tbh + 2);
    display_1330.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display_1330.print(name);
  }
  while (display_1330.nextPage());
}

void myDisplay_1330::update_time(int hour, int min, bool partial) {
  char time[6];

  sprintf(time, "%d:%d", hour, min);

  display_1330.setPartialWindow(250, 120, 300, 100);

  display_1330.setRotation(0);
  display_1330.setFont(&bold_95);
  display_1330.setTextColor(GxEPD_BLACK);

  int16_t tbx_time, tby_time; uint16_t tbw_time, tbh_time;
  display_1330.getTextBounds(time, 0, 0, &tbx_time, &tby_time, &tbw_time, &tbh_time);
  uint16_t x_time = ((display_1330.width() - tbw_time) / 2) - tbx_time;
  uint16_t y_time = ((display_1330.height() - tbh_time) / 2) - tby_time - 70;

  do
  {
    display_1330.fillScreen(GxEPD_WHITE);
    display_1330.setCursor(x_time, y_time);
    display_1330.print(time);
  } 
  while (display_1330.nextPage());
  
  if (!partial) {display_1330.refresh();}
}
#endif



/*============================================================================*/
/*                             DISPLAY_750                                    */
/*============================================================================*/



#ifdef DISPLAY_750
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> 
display_750(GxEPD2_750_T7(10, 14, 21, 33)); //CS, DC, RST, BUSY

//display_750(GxEPD2_750_T7(5, 27, 26, 25));

SPIClass spi(HSPI);

void myDisplay_750::init()
{
  spi.begin(12, -1, 11, 5);
  //SPISettings spisettin;
  //display_750.init(115200, true, 2, false, spi, spisettin);
  display_750.init(115200, true, 10, false, spi, SPISettings(500000, MSBFIRST, SPI_MODE0));

  // spi.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS); // remap hspi for EPD (swap pins)
  // display_750.epd2.selectSPI(spi, spisettin);
  pinMode(5, OUTPUT);

  // display_750.init(115200, true, 2, false);

  display_750.setRotation(0);
  display_750.setFont(&bold_95);
  display_750.setTextColor(GxEPD_BLACK);

  /***   Time   ***/
  int16_t tbx_time, tby_time; uint16_t tbw_time, tbh_time;
  display_750.getTextBounds(mhTime, 0, 0, &tbx_time, &tby_time, &tbw_time, &tbh_time);
  uint16_t x_time = ((display_750.width() - tbw_time) / 2) - tbx_time;
  uint16_t y_time = ((display_750.height() - tbh_time) / 2) - tby_time - 70;

  /***   Temp   ***/
  display_750.setFont(&FreeMonoBold24pt7b);
  int16_t tbx_temp, tby_temp; uint16_t tbw_temp, tbh_temp;
  display_750.getTextBounds(_cTemp, 0, 0, &tbx_temp, &tby_temp, &tbw_temp, &tbh_temp);
  int x_temp = ((320 - tbw_temp) / 2) - tbx_temp + 480;
  int y_temp = ((120 - tbh_temp) / 2) - tby_temp;

  /***   Hum   ***/
  int16_t tbx_hum, tby_hum; uint16_t tbw_hum, tbh_hum;
  display_750.getTextBounds(_cHum, 0, 0, &tbx_hum, &tby_hum, &tbw_hum, &tbh_hum);
  int x_hum = ((320 - tbw_hum) / 2) - tbx_hum;
  int y_hum = ((120 - tbh_hum) / 2) - tby_hum;

  /***   CO2   ***/
  int16_t tbx2_co2, tby2_co2; uint16_t tbw2_co2, tbh2_co2;
  display_750.getTextBounds(CO2, 0, 0, &tbx2_co2, &tby2_co2, &tbw2_co2, &tbh2_co2);

  /***   TVOC   ***/
  int16_t tbx2_tvoc, tby2_tvoc; uint16_t tbw2_tvoc, tbh2_tvoc;
  display_750.getTextBounds(TVOC, 0, 0, &tbx2_tvoc, &tby2_tvoc, &tbw2_tvoc, &tbh2_tvoc);

  /***   CO   ***/
  int16_t tbx2_co, tby2_co; uint16_t tbw2_co, tbh2_co;
  display_750.getTextBounds(CO, 0, 0, &tbx2_co, &tby2_co, &tbw2_co, &tbh2_co);

  display_750.setFullWindow();
  display_750.firstPage();

  do
  {
    display_750.fillScreen(GxEPD_WHITE);

    /***   print Time   ***/
    display_750.setFont(&bold_95);
    display_750.setCursor(x_time, y_time);
    display_750.print(mhTime);

    /***   print Temp   ***/
    display_750.setFont(&FreeMonoBold24pt7b);
    display_750.setCursor(x_temp, y_temp);
    display_750.print(_cTemp);

    /***   print Hum   ***/
    display_750.setCursor(x_hum, y_hum);
    display_750.print(_cHum);

    /***   Balken für CO2   ***/
    int x   = 100;
    int w   = 605;
    int h   =  50;
    int y_1 = 290;
    int y_2 = 350;
    int y_3 = 410;

    display_750.fillRect(x    , y_1    , w     , h     , GxEPD_BLACK);
    display_750.fillRect(x + 5, y_1 + 5, w - 10, h - 10, GxEPD_WHITE);

    display_750.fillRect(x    , y_2    , w     , h     , GxEPD_BLACK);
    display_750.fillRect(x + 5, y_2 + 5, w - 10, h - 10, GxEPD_WHITE);

    display_750.fillRect(x    , y_3    , w     , h     , GxEPD_BLACK);
    display_750.fillRect(x + 5, y_3 + 5, w - 10, h - 10, GxEPD_WHITE);

    /***   print CO2 text    ***/
    display_750.setCursor(x + w - tbw2_co2 - 15, y_3 + tbh2_co2 + 8);
    display_750.print(CO2);

    /***   print TVOC text   ***/
    display_750.setCursor(x + w - tbw2_tvoc - 15, y_2 + tbh2_tvoc + 8);
    display_750.print(TVOC);

    /***   print CO   text   ***/
    display_750.setCursor(x + w - tbw2_co - 15, y_1 + tbh2_co + 8);
    display_750.print(CO);

    display_750.drawXBitmap(0, 325, happy_smiley_bits, 100, 100, GxEPD_BLACK);
    display_750.drawXBitmap(702, 325, sad_smiley_bits, 100, 100, GxEPD_BLACK);
  }
  while (display_750.nextPage());

  delay(1000);
  updateSpace_1("hum:34.6%");
  updateSpace_2("temp:23.8C");
  updateSpace_3("CO2",   220,  100,  250);
  updateSpace_4("CO",     13,   10,   20);
  updateSpace_5("TVOC", 1445, 1000, 2000);
  //update_time(11, 33, false);

  display_750.hibernate();
}

void myDisplay_750::deepSleep() 
{
  display_750.hibernate();
}

void myDisplay_750::printBox(bool isBlack, uint16_t x, uint16_t y, uint16_t w, 
                         uint16_t h, bool partial) {
  display_750.setRotation(0);

  if (partial) { display_750.setPartialWindow(x, y, w - 3, h); }
  else { display_750.setFullWindow(); }

  display_750.firstPage();
  do
  {
    display_750.fillScreen(isBlack ? GxEPD_WHITE : GxEPD_BLACK);
    display_750.fillRect(x, y, w, h, isBlack ? GxEPD_BLACK : GxEPD_WHITE);
  }
  while (display_750.nextPage());
}

uint16_t myDisplay_750::map(uint16_t x, uint16_t IN_min, uint16_t IN_max, 
                        uint16_t OUT_min, uint16_t OUT_max)
{ return ((((x - IN_min)*(OUT_max - OUT_min))/(IN_max - IN_min)) + OUT_min); }

void myDisplay_750::updateSpace_1(const char *val_space_1) {
  display_750.setRotation(0);
  display_750.setTextColor(GxEPD_BLACK);
  display_750.setFont(&FreeMonoBold24pt7b);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display_750.getTextBounds(val_space_1, 0, 0, &tbx, &tby, &tbw, &tbh);
  int x = ((320 - tbw) / 2) - tbx;
  int y = ((120 - tbh) / 2) - tby;

  display_750.setPartialWindow(0, 0, 320, 120);

  do
  {
    display_750.fillScreen(GxEPD_WHITE);
    display_750.setCursor(x, y);
    display_750.print(val_space_1);
  }
  while (display_750.nextPage());
}

void myDisplay_750::updateSpace_2(const char *val_space_2) {
  display_750.setRotation(0);
  display_750.setTextColor(GxEPD_BLACK);
  display_750.setFont(&FreeMonoBold24pt7b);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display_750.getTextBounds(val_space_2, 0, 0, &tbx, &tby, &tbw, &tbh);
  int x = ((320 - tbw) / 2) - tbx + 480;
  int y = ((120 - tbh) / 2) - tby;

  display_750.setPartialWindow(480, 0, 320, 120);

  do
  {
    display_750.fillScreen(GxEPD_WHITE);
    display_750.setCursor(x, y);
    display_750.print(val_space_2);
  }
  while (display_750.nextPage());
}

void myDisplay_750::updateSpace_3(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, 580);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display_750.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display_750.setRotation(0);
  display_750.setPartialWindow(115, 297, 580, 36); // x, y, w, h

  display_750.firstPage();
  do
  {
    display_750.fillScreen(GxEPD_WHITE);
    display_750.fillRect(110, 297, new_w, 36, GxEPD_BLACK);
    display_750.setCursor(117 + (unter_haelfte ? 0 : 580 - tbw - 4), 297 + tbh);
    display_750.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display_750.print(name);
  }
  while (display_750.nextPage());
}

void myDisplay_750::updateSpace_4(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, 580);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display_750.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display_750.setRotation(0);
  display_750.setPartialWindow(115, 357, 580, 36); // x, y, w, h

  display_750.firstPage();
  do
  {
    display_750.fillScreen(GxEPD_WHITE);
    display_750.fillRect(110, 357, new_w, 36, GxEPD_BLACK);
    display_750.setCursor(117 + (unter_haelfte ? 0 : 580 - tbw - 4), 357 + tbh + 2);
    display_750.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display_750.print(name);
  }
  while (display_750.nextPage());
}

void myDisplay_750::updateSpace_5(const char *name, const float wert, const float min, const float max) {
  uint16_t new_w;
  bool unter_haelfte = !((((max - min) / 2) + min) > wert); 
  
  constrain(wert,min,max);
  new_w = map(wert, min, max, 0, 580);
  
  int16_t tbx, tby; uint16_t tbw, tbh;
  display_750.getTextBounds(name, 0, 0, &tbx, &tby, &tbw, &tbh);

  display_750.setRotation(0);
  display_750.setPartialWindow(115, 417, 580, 36); // x, y, w, h

  display_750.firstPage();
  do
  {
    display_750.fillScreen(GxEPD_WHITE);
    display_750.fillRect(110, 417, new_w, 36, GxEPD_BLACK);
    display_750.setCursor(117 + (unter_haelfte ? 0 : 580 - tbw - 4), 417 + tbh + 2);
    display_750.setTextColor(unter_haelfte ? GxEPD_WHITE : GxEPD_BLACK);
    display_750.print(name);
  }
  while (display_750.nextPage());
}

void myDisplay_750::updateTime(int hour, int min, bool partial) {
  char time[6];

  sprintf(time, "%d:%d", hour, min);

  display_750.setPartialWindow(250, 120, 300, 100);

  display_750.setRotation(0);
  display_750.setFont(&bold_95);
  display_750.setTextColor(GxEPD_BLACK);

  int16_t tbx_time, tby_time; uint16_t tbw_time, tbh_time;
  display_750.getTextBounds(time, 0, 0, &tbx_time, &tby_time, &tbw_time, &tbh_time);
  uint16_t x_time = ((display_750.width() - tbw_time) / 2) - tbx_time;
  uint16_t y_time = ((display_750.height() - tbh_time) / 2) - tby_time - 70;

  do
  {
    display_750.fillScreen(GxEPD_WHITE);
    display_750.setCursor(x_time, y_time);
    display_750.print(time);
  } 
  while (display_750.nextPage());
  
  if (!partial) {display_750.refresh();}
}
#endif