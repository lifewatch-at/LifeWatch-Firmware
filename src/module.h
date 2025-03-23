#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <json.h>

#include "SparkFun_Particle_Sensor_SN-GCJA5_Arduino_Library.h" //PM
#include "Adafruit_AGS02MA.h"  //TVOC
#include "SensirionI2cScd4x.h" //CO2
#include "SensirionI2CSgp41.h" //NOx

const char* TAGM = "modules";

#define SHT41_ADDRESS	0x44
#define SCD41_ADDRESS	0x62
#define LMP91000_ADDR	0x48
#define ADS1115_ADDR	0x49
#define GCJA5_ADDRESS	0x33
#define VEML6031_ADDR	0x29
#define SGP41_ADDRESS	0x59
#define AGS02MA_ADDR	0x1A
#define NOISE_ADDRESS	0x48
#define SPEAKER_ADDR	0x4c

// Command definitions (based on datasheet)
#define CMD_START_PERIODIC_MEASUREMENT  0x21B1
#define CMD_READ_MEASUREMENT            0xEC05
#define CMD_STOP_PERIODIC_MEASUREMENT   0x3F86

/*  Messwerte  */

float temp   = 666.6;
float hum    = 666.6;
float pm2_5  = 666.6;
float co_ppm = 666.6;
float co2    = 666.6;

int noise_db   = 666.6;

uint32_t tvoc    = 666;
uint16_t srawNox = 666;

/*  Sensor-Objekte  */

Adafruit_AGS02MA     ags;
SFE_PARTICLE_SENSOR AirS;
SensirionI2CSgp41  sgp41;

void temp_read   (Sensor* sensor);
void hum_read    (Sensor* sensor);
void co2_read    (Sensor* sensor);
void speaker_read(Sensor* sensor);
void co_read     (Sensor* sensor);
void pm_read     (Sensor* sensor);
void tvoc_read   (Sensor* sensor);
void light_read  (Sensor* sensor);
void nox_read    (Sensor* sensor);
void noise_read  (Sensor* sensor);

bool nox_read_was_called     = false;

struct module {
  byte adr;
  void (*func)(Sensor* sensor);
  char name[15];
  char unit[10];
};

// enum NAMES{
// temp, hum, co2, speaker, co, pm, tvoc, light, nox, noise
// };

const module modules[] = {
	{SHT41_ADDRESS	, temp_read	, "Temperature"   , "gr C"   }, // temp
	{SHT41_ADDRESS	, hum_read		, "Humidity"      , "%"      }, // hum
	{SCD41_ADDRESS	, co2_read    , "CO2"           , "µg/m^3" }, // CO2
	{SPEAKER_ADDR	, speaker_read	, "speak"         , "\n"     }, // speaker
	{ADS1115_ADDR	, co_read		, "CO"            , "ppm"    }, // CO
	{GCJA5_ADDRESS	, pm_read		, "PM"            , "ppm"    }, // PM
	{AGS02MA_ADDR	, tvoc_read	, "TVOC"          , "ppb"    }, // TVOC
	{VEML6031_ADDR	, light_read	, "Lightintensity", "Lux"    }, // light
	{SGP41_ADDRESS	, nox_read		, "NOx"           , "ppb"    }, // NOx+
	{NOISE_ADDRESS	, noise_read	, "Noiselevel"    , "dB"     }
};

// TODO restlichen Module hinzu fügen


bool adrTest(byte adr) {
  byte error;

  Wire1.beginTransmission(adr);
  error = Wire1.endTransmission();

  if (error) return false;
  return true; 
}

void read_modules(Device *device) {
	for (int i = 0; i < (sizeof(modules) / sizeof(*modules)); i++) {
		if (adrTest(modules[i].adr)) {
			Sensor sensor;
			modules[i].func(&sensor);
			device->add(sensor);
			ESP_LOGI(TAGM, "Modul gefunden:\t%s", modules[i].name);
		}
	}
}   



/* ------------- module ------------- */


void noise_read(Sensor* sensor) {
	Wire1.setClock(80000);
	Wire1.beginTransmission(0x48);
	Wire1.write(0x0A);
	Wire1.endTransmission();
	Wire1.requestFrom(0x48, 1);
	delay(50);
//  noise_db = (int)Wire1.read();
	sensor->fill("Noiselevel", (int)Wire1.read(), "dB");
	Wire.setClock(400000);
}

void temp_read(Sensor* sensor) {
	Wire1.beginTransmission(0x44);
	Wire1.write(0xFD);
	Wire1.endTransmission();
	delay(20);

	Wire1.requestFrom(0x44, 6);  // 6 Byte = T_MSB, T_LSB, T_CRC, H_MSB, H_LSB, H_CRC
	if (Wire1.available() == 6) {
		uint16_t rawTemp = (Wire1.read() << 8) | Wire1.read(); Wire1.read(); // + CRC ignoriert
		uint16_t rawHum  = (Wire1.read() << 8) | Wire1.read(); Wire1.read();

		// Umrechnung laut Sensirion-Datenblatt
		temp = -45 + 175 * ((float)rawTemp / 65535.0);
		hum  = 100 * ((float)rawHum / 65535.0);
	}
	Wire1.endTransmission();
	sensor->fill("Temperature", temp, "gr C");
}

void hum_read(Sensor* sensor) {
	/*
	Adafruit_SHT4x sht4 = Adafruit_SHT4x();
	sensors_event_t humidity, temperature;

	if (! sht4.begin()) {
	    Serial.println("\nERROR - hum_read()");
	}

	sht4.setPrecision(SHT4X_HIGH_PRECISION);
	sht4.setHeater(SHT4X_NO_HEATER);

	sht4.getEvent(&humidity, &temperature);

	hum  = humidity.relative_humidity;
	*/
	sensor->fill("Humidity", hum, "%");
}

void co2_read(Sensor* sensor) {
	// Start periodic measurement
	Wire1.beginTransmission(SCD41_ADDRESS);
	Wire1.write(CMD_START_PERIODIC_MEASUREMENT >> 8);   // Send MSB
	Wire1.write(CMD_START_PERIODIC_MEASUREMENT & 0xFF); // Send LSB
	Wire1.endTransmission();

	// Read measurement data
	Wire1.beginTransmission(SCD41_ADDRESS);
	Wire1.write(CMD_READ_MEASUREMENT >> 8);   // Send MSB
	Wire1.write(CMD_READ_MEASUREMENT & 0xFF); // Send LSB
	Wire1.endTransmission();

	delay(1); // Wait for the command execution time (1 ms)

	Wire1.requestFrom(SCD41_ADDRESS, 9); // Request 9 bytes (CO2, Temp, RH, and CRCs)

	if (Wire1.available() == 9) {
		uint16_t co2_raw = Wire1.read() << 8 | Wire1.read();
		Wire1.read(); // Skip CRC

		uint16_t temperature_raw = Wire1.read() << 8 | Wire1.read();
		Wire1.read(); // Skip CRC

		uint16_t humidity_raw = Wire1.read() << 8 | Wire1.read();
		Wire1.read(); // Skip CRC

		float _temperature = -45.0 + 175.0 * ((float)temperature_raw / 65535.0);
		float _humidity = 100.0 * ((float)humidity_raw / 65535.0);

		co2 = co2_raw;//(float)(co2_raw * (44.01*1000/24,45));
	} else {
		ESP_LOGW(TAGM, "Failed to read data from SCD41.");
	}
}

void speaker_read(Sensor* sensor) {

}


#define RTIA_OHMS           120000.0
#define RL_OHMS             100.0
#define SENSITIVITY_NA_PPM  25.0
#define ZERO_CURRENT_NA     0.0
#define ADC_LSB_V           0.000125  // für ±4.096V

float voltage = 0, current_nA = 0;

void writeLMP(uint8_t reg, uint8_t value) {
	Wire1.beginTransmission(LMP91000_ADDR);
	Wire1.write(reg);
	Wire1.write(value);
	Wire1.endTransmission();
	delay(10);
}

int16_t readADS1115() {
	// 1. Config-Register setzen (Single-Ended AIN0, GAIN=1, 128 SPS)
	Wire1.beginTransmission(ADS1115_ADDR);
	Wire1.write(0x01); // Config-Register
	Wire1.write(0b11000010); // MSB: OS=1, MUX=100 (AIN0), PGA=001 (±4.096V), MODE=1
	Wire1.write(0b11100011); // LSB: 128 SPS, Disable Comparator
	Wire1.endTransmission();
  
	delay(9); // Warten bis ADC fertig (~7.8ms @128SPS)
  
	// 2. Conversion-Register lesen (0x00)
	Wire1.beginTransmission(ADS1115_ADDR);
	Wire1.write(0x00); // Conversion register
	Wire1.endTransmission();
  
	Wire1.requestFrom(ADS1115_ADDR, 2);
	int16_t result = 0;
	if (Wire1.available() == 2) {
	  result = (Wire1.read() << 8) | Wire1.read();
	}
	return result;
  }

void co_read(Sensor* sensor) {
	Wire1.setClock(100000);
	writeLMP(0x01, 0x00);  // Unlock
	writeLMP(0x12, 0x03);  // MODECN: 3-Elektroden-Modus
	writeLMP(0x10, 0x1B);  // TIACN: RTIA=120kΩ, RL=100Ω
	writeLMP(0x11, 0x01);  // REFCN: VREF=20%

	int16_t adc = readADS1115();

	voltage = adc * ADC_LSB_V;
	current_nA = ((voltage / RTIA_OHMS) / RL_OHMS) * 1e9;
	co_ppm = (current_nA - ZERO_CURRENT_NA) / SENSITIVITY_NA_PPM;

	Wire1.setClock(400000);
}

void pm_read(Sensor* sensor) {
	if (AirS.begin(Wire1) == false) {
		Serial.println("\nERROR - pm_read()");
	}
	pm2_5 = AirS.getPM2_5();
}

void tvoc_read(Sensor* sensor) {   
	// Wire1.setClock(20000);

	// Wire1.beginTransmission(0x1A);
	// Wire1.write(0x00);  // Messung starten
	// Wire1.endTransmission();

	// delay(150);

	// Wire1.requestFrom(0x1A, 4);

	// if (Wire1.available() < 4) {
	//     Serial.println("I2C Read Error! - TVOC");
	// }

	// uint16_t gasRaw   = (Wire1.read() << 8) | Wire1.read();   // Gas Resistance
	// // float gasKOhms = gasRaw / 1000.0;  // laut Datenblatt Umrechnung in kOhm
	// uint16_t tvocRaw  = (Wire1.read() << 8) | Wire1.read();  // TVOC

	// if (tvocRaw == 0xFFFF || tvocRaw > 3000) {
	//     Wire1.setClock(400000);
	//     return;
	// }

	// tvoc = tvocRaw;
	// Wire1.setClock(400000);
}

void light_read(Sensor* sensor) {

}

void nox_read(Sensor* sensor) {
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
		return;
	}
	srawNox /= 1000;
}