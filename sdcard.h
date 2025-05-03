
#ifndef SD_H
#define SD_H

#include "SD.h"
#include "FS.h"
#include <SPI.h>
#include <Arduino.h>

extern class Preferences preferences;

extern void sd_check();
extern void alarm_led(uint16_t x, byte z);
extern void logger_time(void);
extern void appendFile(fs::FS& fs, const char* path, const char* message);
extern void writeFile(fs::FS& fs, const char* path, const char* message);
extern String readFile(fs::FS& fs, const char* path);
extern const uint8_t SDchipSelect;
extern byte z;
extern uint16_t x;
extern String readssid, readpass;
extern char filename[16];
extern char stats_filename[20];
extern char ssidfilename[20];
extern char passfilename[20];
extern bool diff_save_sd;
extern bool auto_save_sd;
extern char buf3[10];  //ampHours
extern char buf4[10];  //power_W
extern char buf5[10];  //wattHours

extern bool AutomaticEndOfCharging;

extern double kp;      // Współczynnik proporcjonalny
extern double ki;  // Współczynnik całkujący
extern double kd;       // Współczynnik różniczkujący
extern uint16_t pidValue;










#endif
