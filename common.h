
#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include "AiEsp32RotaryEncoder.h"
//----------------------


#define MIN_BAT_VOLTS 5.0  // minimalne napięcie aku potrzebne do załączenia ładowarki
#define NOM_BAT_VOLTS 12.0
#define BULK_BAT_VOLTS 12.4   // początkowy prąd ładowania przy b. rozładowanym aku
#define TIME_FLOAT 1         // maksymalny czas trwania trybu float w minutach
#define TIME_ABSORPTION 1440  // maksymalny czas trwania trybu abs w minutach
#define TIME_STORAGE 1

#define ERROR_LED 2
#define CHARGE_PIN 27
#define DISCHARGE_PIN 26
#define FAN_PIN 33

#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 15
#define ROTARY_ENCODER_BUTTON_PIN 17
#define ROTARY_ENCODER_STEPS 4

#define PWM_MIN 1

//#define R_SHUNT 0.11
#define V_SHUNT_MAX 0.32
#define V_BUS_MAX 32
#define I_MAX_EXPECTED 3.2

#define WIFI_FUNC 1
#define DEBUG_SD 1
#define DEBUG_READ_DATA 0
#define THINGSPEAK 0
#define PID 1

extern const char* MyHostName;
extern bool SerialPort;
extern void bat_init(void);
extern const char* COMPILATION;

// build_version.h
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define VERSION_BUILD 42  // będzie zwiększane automatycznie

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define VERSION_STRING \
  TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH) "-" TOSTRING(VERSION_BUILD)

typedef struct {
  float u_bulk;
  float u_float;
  float u_recond;
  float u_storage;
  float capacity;
} TBATT;

typedef struct {
  uint16_t seconds;
  uint16_t minutes;
  uint16_t hours;
  uint32_t full_minutes;  // niezerowana wersja, np. do porównań
  String formatted;
} ChargerTime;

extern ChargerTime chargerTime[6];

typedef enum ChargingType {
    initial_charging = 19,
    normal_charging = 10,
    fast_charging = 5,
    recond_charging = 9,
    asymetry_charging = 20
};
extern ChargingType charging_type;

typedef enum battery_mode { battery_none,
                            battery_std,
                            battery_gel,
                            battery_agm,
                            battery_caca,
                            battery_li_2s,
                            battery_li_3s } TBATTERY_TYPE;
extern TBATTERY_TYPE battery_type;

typedef enum charger_mode { off,
                            initial,
                            bulk,
                            absorption,
                            storage,
                            bat_float,
                            bat_disch 
                            } TCHARGER_STATE;
extern TCHARGER_STATE charger_state;

extern uint8_t abs_little_intervals;
extern uint8_t abs_big_intervals;

//extern float PID_errSum, lastErr, output;
extern unsigned long lastTime;

extern int selectedItem;
extern uint8_t topItem;
extern const uint8_t totalItemsP14;
extern const uint8_t visibleItems;
extern const char* menuItemsP14[7];

extern struct tm timeinfo;
extern void printLocalTime();

extern unsigned long myChannelNumber;
extern const char* myWriteAPIKey;

extern const char* ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;
extern const char* host;
extern const int UDPin;  // Pin 2 is UP/DOWN
extern const int CSPin;  // Pin 3 is CHIP SELECT

extern class INA219 ina219;
extern class Preferences preferences;
extern class WebServer server;

extern void encoder();
extern void calc_data_time();
extern void mDelay(uint16_t ms);
extern void read_data();
extern void alarm_led(uint16_t x, byte z);
extern void check_wifi_status();
extern void logger_time(void);
extern void mdns();
extern void read_eep();
extern void wifi_connect();
extern void wyslij_speak();
extern void print_serial();
extern void pid_pwm_duty();

extern float alpha;
extern float last_cur_Avg;
extern float last_vol_Avg;
extern float busvoltage;
extern float current_auto;
extern const float diff_amps;
extern const float diff_volts;
extern float power_W;
extern float wattHours;
extern float CurrentDisch;

extern String localTime;

extern uint8_t szerokosc_pola;
extern float liczba;
extern const char* COMPILATION;

extern const char* loginIndex;
extern const char* serverIndex;

extern String Stime_sec, Stime_min, Stime_hour;                                                                                                   //zmienne sformatowane przechowujące czas
extern String czas;                                                                                                                               //zmienna przechowująca czas ładowania HH:MM:SS
extern String czas_bulk, czas_storage, czas_float;                                                                                                //zmienna przechowująca czas trybu bulk
extern String czas_abs;                                                                                                                           //zmienna przechowująca czas trybu absorption
extern String czas_initial;                                                                                                                       //zmienna przechowująca czas trybu initial
extern String localTime;                                                                                                                          //zmienna przechowująca czas z internetu hh:mm:ss
extern String Shours, Smin, Ssec, Shours_initial, Smin_initial, Ssec_initial, Shours_bulk, Smin_bulk, Ssec_bulk, Shours_abs, Smin_abs, Ssec_abs;  //stringi czasowe
extern String ssid, password;
extern String readssid, readpass;  // odczytane z sd

extern float wattSec;
extern float wattHours;
extern float busvoltage;
extern float current_auto;
extern float power_W;
extern float ampHours;
extern float ampHours_m;
extern float ampSec;
extern float ampSec_m;
extern float Rshunt;
extern float umin;
extern float u_bulk;
extern float imax;
extern float ufloat;
extern float i_manual;
extern float u_manual;
extern float pojemnosc;
extern const float u12_auto;
extern float alpha;
extern float last_cur_Avg;
extern float last_vol_Avg;
extern float storage_batt_volts;
extern float initial_Ah;
extern float bulk_Ah;
extern float abs_Ah;
extern float float_Ah;
extern float storage_Ah;
extern float asym_val;
extern float min_voltage_li_2s;
extern float min_voltage_li_3s;

extern float batt_abs_current_1h;
extern float batt_abs_current_3h;
extern float batt_abs_current_5h;
extern float batt_abs_current_6h;
extern float batt_abs_current_7h;
extern float batt_abs_current_9h;
extern float batt_abs_current_11h;
extern float batt_abs_current_12h;
extern float batt_abs_current_15h;

extern const float diff_amps;
extern const float diff_volts;

extern uint32_t msec;          //zmienna potrzebna do wyznaczenia 1 sek
extern uint32_t last_msec;     //zmienna potrzebna do wyznaczenia 1 sek
extern uint16_t elapsed_msec;  //zmienna potrzebna do wyznaczenia 1 sek
extern uint16_t elapsed_time;  //zmienna potrzebna do wyznaczenia 1 sek
extern float cur_sampleRaw;

extern const uint16_t charging_freq;          //częstotliwość pwm
extern const uint16_t discharging_freq;       //częstotliwość pwm
extern const uint8_t charging_resolution;     //rozdzielczość pwm 16384
extern const uint8_t discharging_resolution;  //rozdzielczość pwm 16384
extern const uint8_t pwm_charge_channel;      //kanał pwm
extern const uint8_t pwm_discharge_channel;
extern const uint16_t charging_MAX_DUTY_CYCLE;
extern const uint16_t discharging_MAX_DUTY_CYCLE;

extern const uint8_t SDchipSelect;  //pin SS czytnika SD
//extern uint8_t charging_type;
////extern uint8_t fast_charging;
//extern uint8_t normal_charging;
//extern uint8_t recond_charging;
//extern uint8_t asymetry_charging;

extern float output;
extern uint16_t pidValue;
extern uint16_t pwmValue;
extern uint16_t PWM_MAX;  // warość PWM określająca 100% wypełnienia
extern int pwm;
extern uint16_t pwm1;
extern uint16_t pwm_manual;
extern uint8_t t_charge_auto, t_discharge_auto, t_charge, t_discharge;
extern uint8_t sd_seconds, sd_minutes;
extern uint16_t seconds, minutes, hours;                          //główna zmienna przechowująca czas ładowania
extern uint16_t seconds_bulk, minutes_bulk, hours_bulk;           //zmienna przechowująca czas trybu bulk
extern uint16_t seconds_abs, minutes_abs, hours_abs;         //zmienna przechowująca czas trybu absorption
extern uint16_t seconds_initial, minutes_initial, hours_initial;  //zmienna przechowująca czas trybu initial
extern uint8_t pot_pos;
extern uint16_t minutess_float;
extern uint16_t minutess_storage;

extern uint8_t menuitem;
extern uint16_t page;
extern uint16_t sd_num;
extern bool up;
extern bool down;
extern bool middle;
extern bool start_timer_absorption;
extern bool force_sd;
//bool charging_end;
extern bool flag_start_timer_float;
//bool flag_start_timer_absorption;
extern bool charging_end;
extern bool fan_auto;
extern bool pwm_auto;
extern bool i_auto;
extern bool save_to_sd;    // potwierdzenie zapisu na sd
extern bool auto_save_sd;  // zezwolenie na ciągły zapis sd bez wzgl na tryb
extern bool diff_save_sd;
extern bool current_automode;
extern bool menuitem_stop;  // zmienna blokująca zmiane menuitem podczas in/dekrementacji zmiennych(menu batt settings)
extern bool init_param;
extern bool asym_mode;
extern bool bigbattery;

extern byte time_sec, time_min, time_hour, wifi_status;  //zmienne niesformatowane przechowujące czas i status wifi
extern byte mode_C20;
extern byte mode_C10;
extern byte mode_C8;

extern uint8_t fan_manual;
extern uint16_t sd_interval;
extern uint16_t abs_interval;
extern uint8_t fan_rpm;
extern uint16_t minutess_abs;  //zmienna nie zerowana po przekroczeniu 59
//extern int minutess_abs;

// extern double kp;      // Współczynnik proporcjonalny
// extern double ki;  // Współczynnik całkujący
// extern double kd;       // Współczynnik różniczkujący
extern uint16_t pidValue;

extern double PID_error;
extern double PID_errSum;
extern double PID_dErr;
extern double P;
extern double I;
extern double D;


extern char buf1[10];  //busvoltage
extern char buf2[10];  //current_auto
extern char buf3[10];  //ampHours
extern char buf4[10];  //power_W
extern char buf5[10];  //wattHours
extern char buf6[10];  //localTime

extern char filename[16];  //zmienna dla nazwy pliku
extern char pid_filename[16];
extern char ssidfilename[20];
extern char passfilename[20];
extern char current_filename[20];

#endif
