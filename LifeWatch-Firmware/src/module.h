#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "SparkFun_Particle_Sensor_SN-GCJA5_Arduino_Library.h" //PM
#include "Adafruit_AGS02MA.h"  //TVOC
#include "Adafruit_SHT4x.h"    //Temp / Hum
#include "SensirionI2CScd4x.h" //CO2
#include "SensirionI2CSgp41.h" //NOx

/*  Messwerte  */

float temp  = 666.6;
float hum   = 666.6;
float pm2_5 = 666.6;

uint32_t tvoc    = 66666;
uint16_t srawNox =   666;

/*  Sensor-Objekte  */

Adafruit_AGS02MA     ags;
Adafruit_SHT4x      sht4; // = Adafruit_SHT4x();
SFE_PARTICLE_SENSOR AirS;
SensirionI2CSgp41  sgp41;

void temp_read   (void);
void hum_read    (void);
void co2_read    (void);
void speaker_read(void);
void co_read     (void);
void pm_read     (void);
void tvoc_read   (void);
void light_read  (void);
void nox_read    (void);

bool temp_read_was_called    = false;
bool hum_read_was_called     = false;
bool co2_read_was_called     = false;
bool speaker_read_was_called = false;
bool co_read_was_called      = false;
bool pm_read_was_called      = false;
bool tvoc_read_was_called    = false;
bool light_read_was_called   = false;
bool nox_read_was_called     = false;

struct module {
  byte adr;
  void (*func)(void);
  char name[10];
  char unit[5];
};

const module modules[] = {
  {0x44, temp_read   , "temp" , "gr C"}, // temp
  {0x44, hum_read    , "hum"  , "%"   }, // hum
  {0x62, co2_read    , "CO2"  , "ppm" }, // CO2
  {0x4c, speaker_read, "speak", "\n"  }, // speaker
  {0x49, co_read     , "CO"   , "ppm" }, // CO
  {0x33, pm_read     , "pm"   , "ppm" }, // PM
  {0x1A, tvoc_read   , "TVOC" , "ppb" }, // TVOC
  {0x29, light_read  , "light", "Lux" }, // light
  {0x59, nox_read    , "nox"  , "ppm" }  // NOx
};

/*TODO restlichen Module hinzu fügen */

void module_init(void) {
    Wire1.begin(8, 9, 400000);
}

bool adrTest(byte adr) {
  byte error;

  Wire1.beginTransmission(adr);
  error = Wire1.endTransmission();

  if (error) return false;
  return true; 
}

void module_check(void) {
    for (int i = 2; i < (sizeof(modules) / sizeof(*modules)); i++) {
        Serial.print("\nModul - ");
        Serial.println(modules[i].name);
        if (adrTest(modules[i].adr)) {
            Serial.println("calling func");
            modules[i].func();

            Serial.print("Modul gefunden:\t");
            Serial.println(modules[i].name);
        }
    }

    Serial.print("\n");
}   



/* ------------- module ------------- */



void temp_read(void) {
    Serial.println("Temp_read() - start");

    sensors_event_t humidity, temperature;

    if (!temp_read_was_called) {
        if (! sht4.begin()) {
            Serial.println("\nERROR - temp_read()");
        }
        else {
            temp_read_was_called = true;
        }
    }

    sht4.setPrecision(SHT4X_HIGH_PRECISION);
	sht4.setHeater(SHT4X_NO_HEATER);

    sht4.getEvent(&humidity, &temperature);

    temp = temperature.temperature;
    hum  = humidity.relative_humidity;
}

void hum_read(void) {
    // Adafruit_SHT4x sht4 = Adafruit_SHT4x();
    // sensors_event_t humidity, temperature;

    // if (! sht4.begin()) {
    //     Serial.println("\nERROR - hum_read()");
    // }

    // sht4.setPrecision(SHT4X_HIGH_PRECISION);
	// sht4.setHeater(SHT4X_NO_HEATER);

    // sht4.getEvent(&humidity, &temperature);

    // hum  = humidity.relative_humidity;
}

void co2_read(void) {
    
}

void speaker_read(void) {

}

void co_read(void) {

}

void pm_read(void) {
    if (AirS.begin() == false) {
        Serial.println("\nERROR - pm_read()");
    }
    pm2_5 = AirS.getPM2_5();
}

void tvoc_read(void) {
    Serial.print("\ntvoc_read() - start; value = ");
    Serial.println(tvoc_read_was_called);
    
    if (!tvoc_read_was_called) {    
        if (!ags.begin(&Wire1, 0x1A)) {
            Serial.println("\nERROR - tvoc_read()");
        }
        else {
            tvoc_read_was_called = true;
        }
    }
    tvoc = ags.getTVOC(); // TVOC in ppb
}

void light_read(void) {

}

void nox_read(void) {
    uint16_t conditioning_s = 3; //10;

    if (!nox_read_was_called) {
        sgp41.begin(Wire1);
    }

    uint16_t error;
    char errorMessage[256];
    uint16_t defaultRh = 0x8000;
    uint16_t defaultT = 0x6666;
    uint16_t srawVoc = 0;

    while (conditioning_s > 0) {
        // During NOx conditioning (10s) SRAW NOx will remain 0
        error = sgp41.executeConditioning(defaultRh, defaultT, srawVoc);
        conditioning_s--;
        delay (1000);
    }
    
    // Read Measurement
    error = sgp41.measureRawSignals(defaultRh, defaultT, srawVoc, srawNox);

    if (error) {
        Serial.println("\nERROR - nox_read()");
    }
}