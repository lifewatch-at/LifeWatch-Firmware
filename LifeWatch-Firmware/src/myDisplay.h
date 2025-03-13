#pragma once

#include <Arduino.h>
#include <GxEPD2_BW.h>

/* Fonts */
#include <Fonts/FreeMonoBold24pt7b.h>
#include "Bold95.h"
#include "smileys.h"

class myDisplay
{
public:
  void init();
  void deepSleep();
  void refresh();
  void printLayout();
  void BatteryWarning(bool show_warning);
  void updateTime(int hour, int min, bool partial);
  void updateSpace_1(const char *val_space_1);
  void updateSpace_2(const char *val_space_2);
  void updateSpace_3(const char *name, const float wert, const float min, const float max);
  void updateSpace_4(const char *name, const float wert, const float min, const float max);
  void updateSpace_5(const char *name, const float wert, const float min, const float max);
  void updateEverything(int hour = 0, int min = 0, const char* space_1 = "", const char* space_2 = "",
    const char *name_space_3 = "", const float value_space_3 = 0, const float min_space_3 = 0, const float max_space_3 = 100,
    const char *name_space_4 = "", const float value_space_4 = 0, const float min_space_4 = 0, const float max_space_4 = 100,
    const char *name_space_5 = "", const float value_space_5 = 0, const float min_space_5 = 0, const float max_space_5 = 100);

private:
  uint16_t map(uint16_t x, uint16_t IN_min, uint16_t IN_max, uint16_t OUT_min, uint16_t OUT_max);
  void printBox(bool isBlack, uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial);
  
  char _cTemp[7]  = "28.3 C";
  char _cHum [4]  = "36%";
  char mhTime[6]  = "12:45";
  char CO2   [8]  = "CO2";
  char TVOC  [5]  = "TVOC";
  char CO    [3]  = "CO";
};