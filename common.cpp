
#include <Arduino.h>
#include <stdlib.h>
#include <INA219.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include "AiEsp32RotaryEncoder.h"
#include "time.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "ThingSpeak.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <Preferences.h>


//-----------------------------------------------------------------------------------------------------------------------//


#include "display.h"
#include "check_page.h"
#include "common.h"
#include "sdcard.h"


//-----------------------------------------------------------------------------------------------------------------------//

const char* COMPILATION = "1.0";
bool SerialPort = 1;
const char* MyHostName = "ESP32-Charger";

extern class INA219 ina219;
extern class AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
extern class Preferences preferences;
extern class WiFiClient client;

TBATTERY_TYPE battery_type;
TCHARGER_STATE charger_state;

TBATT battery[7];


const char* ntpServer = "pool.ntp.org";
//const long gmtOffset_sec = 3600;
const long gmtOffset_sec = 7200;
const int daylightOffset_sec = 0;

const char* host = "esp32";
const int UDPin = 13;  // Pin 2 is UP/DOWN
const int CSPin = 12;  // Pin 3 is CHIP SELECT Ssec_float



const uint8_t totalItemsP14 = 7;
const char* menuItemsP14[totalItemsP14] = {
  "FAN", "PWM", "SD", "BATT SETTINGS", "SHUNT", "PID", "EXIT"
};
const uint8_t visibleItems = 4;
uint8_t menuitem = 1;  //Aktualnie wybrana pozycja menu
uint8_t topItem = 0;   // Pierwsza widoczna pozycja menu
uint16_t page;
uint16_t sd_num = 0;
bool up = false;
bool down = false;
bool middle = false;

bool bigbattery = false;
bool DischarStart = false;
bool DischarSet = false;


String Stime_sec, Stime_min, Stime_hour;                                                                                                   //zmienne sformatowane przechowujące czas
String czas;                                                                                                                               //zmienna przechowująca czas ładowania HH:MM:SS
String czas_bulk, czas_storage, czas_float;                                                                                                //zmienna przechowująca czas trybu bulk
String czas_abs;                                                                                                                           //zmienna przechowująca czas trybu absorption
String czas_initial;                                                                                                                       //zmienna przechowująca czas trybu initial
String localTime;                                                                                                                          //zmienna przechowująca czas z internetu hh:mm:ss
String Shours, Smin, Ssec, Shours_initial, Smin_initial, Ssec_initial, Shours_bulk, Smin_bulk, Ssec_bulk, Shours_abs, Smin_abs, Ssec_abs;  //stringi czasowe
String Shours_storage, Smin_storage, Ssec_storage, Shours_float, Smin_float, Ssec_float;
String ssid, password;
String readssid, readpass;  // odczytane z sd


float wattSec = 0.00;
float wattHours = 0.00;
float busvoltage = 0.00;
float current_auto = 0.00;
float power_W = 0.000;
float ampHours = 0.000;
float ampHours_m = 0.000;
float ampSec = 0.000;
float ampSec_m = 0.000;
float Rshunt = 0.10;
float umin = 10.50;
float u_bulk = 14.50;
float imax = 0.5;
float ufloat = 13.20;
float i_manual = 1.20;
float u_manual = 11.00;
float pojemnosc = 4;
const float u12_auto = 12.00;
float alpha = 0.8;
float last_cur_Avg = 0.00;
float last_vol_Avg = 0.00;
float storage_batt_volts = 12.6;
float initial_Ah;
float bulk_Ah;
float abs_Ah;
float float_Ah;
float storage_Ah;
float test_capacity = 10.0;

float min_voltage_li_2s = 5;
float min_voltage_li_3s = 7.0;

const float diff_amps = 0.05;
const float diff_volts = 0.05;

uint32_t msec = 0;          //zmienna potrzebna do wyznaczenia 1 sek
uint32_t last_msec = 0;     //zmienna potrzebna do wyznaczenia 1 sek
uint16_t elapsed_msec = 0;  //zmienna potrzebna do wyznaczenia 1 sek
uint16_t elapsed_time = 0;  //zmienna potrzebna do wyznaczenia 1 sek

const uint16_t charging_freq = 5000;     //częstotliwość pwm
const uint8_t pwm_charge_channel = 1;    //kanał pwm
const uint8_t charging_resolution = 12;  //rozdzielczość pwm 1024
const uint16_t charging_MAX_DUTY_CYCLE = (int)(pow(2, charging_resolution) - 1);
float asym_val = 10.0;

const uint16_t discharging_freq = 5000;     //częstotliwość pwm
const uint8_t pwm_discharge_channel = 2;    //kanał pwm
const uint8_t discharging_resolution = 12;  //rozdzielczość pwm 1024
const uint16_t discharging_MAX_DUTY_CYCLE = (int)(pow(2, discharging_resolution) - 1);
const uint8_t SDchipSelect = 5;  //pin SS czytnika SD

uint16_t PWM_MAX = 1024;  // warość PWM określająca 100% wypełnienia
int pwm;
uint16_t pwm1;
uint16_t pwm_manual;
uint8_t t_charge_auto, t_discharge_auto, t_charge, t_discharge;
uint8_t abs_min_intervals = 26;
uint8_t abs_h_intervals = 18;


uint16_t seconds = 0, minutes = 0, hours = 0;
uint16_t seconds_initial, minutes_initial, hours_initial;  //zmienna przechowująca czas trybu initial
uint16_t seconds_bulk, minutes_bulk, hours_bulk;           //zmienna przechowująca czas trybu bulk
uint16_t seconds_abs, hours_abs, minutes_abs;              //zmienna przechowująca czas trybu absorption
uint16_t seconds_storage, minutes_storage, hours_storage;  //zmienna przechowująca czas trybu storage
uint16_t seconds_float, minutes_float, hours_float;        // zmienna inkrementowana bez ograniczenia 59 min
uint16_t minutess_abs;
uint16_t minutess_storage, minutess_float;
uint8_t sd_seconds = 0;
uint8_t sd_minutes = 0;

float cur_sampleRaw = 0.00;  // odczytana wartość prądu z INA
float CurrentDisch;          // prąd rozładowania
float dischVoltage = 10.5;

bool start_timer_absorption = false;
bool force_sd = false;
bool flag_start_timer_float = false;
bool use_min_intervals = true;
bool ChargingMode = false;
bool DischargingMode = false;
bool charging_end = true;
bool fan_auto = true;
bool pwm_auto = true;
bool i_auto = true;
bool save_to_sd = false;   // potwierdzenie zapisu na sd
bool auto_save_sd = true;  // zezwolenie na ciągły zapis sd bez wzgl na tryb
bool diff_save_sd = false;
bool current_automode = true;
bool menuitem_stop = false;  // zmienna blokująca zmiane menuitem podczas in/dekrementacji zmiennych(menu batt settings)
bool init_param = true;
bool asym_mode = false;


byte time_sec, time_min, time_hour, wifi_status;  //zmienne niesformatowane przechowujące czas i status wifi
// byte mode_C20 = 20;
// byte mode_C10 = 10;
// byte mode_C8 = 8;

uint8_t fan_manual = 60;
uint16_t sd_interval = 1000;
uint16_t abs_interval = 5000;
uint8_t fan_rpm = 50;

// PID stałe
double kp = 1000.0;
double ki = 5.0;
double kd = -4.0;

// PID zmienne
double PID_error = 0.0;
double PID_errSum = 0.0;
double PID_dErr = 0.0;
double P, I, D;
float lastErr = 0.0;
unsigned long lastTime = 0;
uint16_t pidValue = 0;

float setPoint = 14.5;  // Zadane napięcie (w woltach)
float input, output;


uint16_t pwmValue;

char buf1[10];  //busvoltage
char buf2[10];  //current_auto
char buf3[10];  //ampHours
char buf4[10];  //power_W
char buf5[10];  //wattHours
char buf6[10];  //localTime

char filename[16];  //zmienna dla nazwy pliku
char pid_filename[16];
char stats_filename[20];
char ssidfilename[20];
char passfilename[20];
char current_filename[20];

uint8_t abs_elapsed_msec;
uint8_t abs_msec;
uint8_t abs_last_msec;
uint8_t abs_elapsed_time;
float abs_time;




unsigned long myChannelNumber = 1074127;
const char* myWriteAPIKey = "OC9W1CY1OQFQAH8I";

unsigned long previousMillis = 0;  // Zmienna do przechowywania poprzedniego stanu millis
const long interval = 1000;        // Interwał czasowy w milisekundach (1 sekunda)

void bat_init() {
  // battery[0] jest domyślnie wybrane po resecie
  battery[1].u_bulk = 14.4;
  battery[1].u_float = 13.2;
  battery[1].u_recond = 14.8;
  battery[1].u_storage = 13.8;
  battery[1].capacity = 4.0;  // std battery
  battery[2].u_bulk = 14.4;
  battery[2].u_float = 13.2;
  battery[2].u_recond = 14.5;
  battery[2].u_storage = 13.8;
  battery[2].capacity = 5.0;  // Gel battery
  battery[3].u_bulk = 14.4;
  battery[3].u_float = 13.2;
  battery[3].u_recond = 14.9;
  battery[3].u_storage = 13.8;
  battery[3].capacity = 4.0;  // Agm battery
  battery[4].u_bulk = 14.8;
  battery[4].u_float = 13.2;
  battery[4].u_recond = 16.2;
  battery[4].u_storage = 13.8;
  battery[4].capacity = 40.0;  // CaCa battery
  battery[5].u_bulk = 8.4;
  battery[5].u_float = 8.4;
  battery[5].u_recond = 8.4;
  battery[5].u_storage = 8.4;
  battery[5].capacity = 2.0;  // Li-ion 2s battery
  battery[6].u_bulk = 12.6;
  battery[6].u_float = 12.6;
  battery[6].u_recond = 12.6;
  battery[6].u_storage = 12.6;
  battery[6].capacity = 2.0;  // Li-ion 3s battery
  init_param = false;
}

///////////////////////////// Pomiar i formatowanie danych wyjściowych ////////////////////////////

float PidDeadZone(float input, float target, float maxTolerance) {
  float error = target - input;

  // Sprawdzamy, czy różnica mieści się w dodatniej strefie martwej
  if (error >= 0.0 && error <= maxTolerance) {
    return 0.0;  // Brak reakcji PID w dodatniej strefie martwej
  }
  return (error);  // Zwrot błędu, jeśli wykracza poza strefę martwą
}

void read_data() {

  float cur_sample = 0.00;
  float cur_avg = 0.00;
  float vol_sample = 0.00;
  //float vol_avg = 0.00;
  //float targetVoltage;

  //float deadZoneTolerance = 0.01;                      // Ustawienie tolerancji (np. ±0.05V)
  //float inputVoltage = vol_avg;                        // Aktualne napięcie zmierzone
  float targetVoltage = battery[battery_type].u_bulk;  // Cel napięcia

  // switch (charger_state) {
  //   case absorption:
  //     targetVoltage = battery[battery_type].u_bulk;
  //     break;
  //   case storage:
  //     targetVoltage = battery[battery_type].u_storage;
  //     break;
  //   case bat_float:
  //     targetVoltage = battery[battery_type].u_float;
  //     break;
  //   default:
  //     targetVoltage = 0;  // lub np. battery[battery_type].u_bulk jako domyślne
  //     break;
  // }

  //++++++++++++++++++++++ VOLTAGE +++++++++++++++++++++++++++

  vol_sample = ina219.busVoltage();

  float static vol_avg = 0.0;
  vol_avg = (vol_sample * alpha) + (last_vol_Avg * (1.0f - alpha));
  busvoltage = vol_avg;

  if (PID) {
    // Czas od ostatniej iteracji
    unsigned long now = millis();


    float timeChange = (float)(now - lastTime) / 1000.0f;

    /////////////////////////////// Wykonanie algorytmu PID ///////////////////////////////////

    if (charger_state == absorption) {
      pid_pwm_duty();
      PID_error = battery[battery_type].u_bulk - vol_avg;
    }
    if (charger_state == storage) {
      kp = 500;
      pid_pwm_duty();
      PID_error = battery[battery_type].u_storage - vol_avg;
    }

    if (charger_state == storage) {
      kp = 500;
      pid_pwm_duty();
      PID_error = battery[battery_type].u_float - vol_avg;
    }


    //PID_error = targetVoltage - vol_avg;

    // Obliczenie błędu




    //PID_error = PidDeadZone(busvoltage, targetVoltage, deadZoneTolerance);
    //   if (abs(PID_error) < deadZoneTolerance) {
    //   PID_error = 0.0;
    // }
    if (timeChange > 0.0) {

      //Obliczenia PID
      PID_errSum += PID_error * timeChange;


      // Anti-windup
      float maxErrSum = 200.0;  // dostosuj do systemu
      PID_errSum = constrain(PID_errSum, -maxErrSum, maxErrSum);

      PID_dErr = (PID_error - lastErr) / timeChange;

      P = kp * PID_error;
      I = ki * PID_errSum;
      D = kd * PID_dErr;

      output = P + I + D;

      // Aktualizacja i ograniczenie wyjścia
      pidValue = constrain((int)output, 0, 512);

      // Aktualizacja do następnej iteracji
      lastErr = PID_error;
      lastTime = now;
    }
  }


  if (fabs(busvoltage - vol_avg) >= 0.1) {
    diff_save_sd = true;
  } else if (fabs(busvoltage - last_vol_Avg) <= 0.1) {
    if (diff_save_sd == false)
      if (sd_minutes == 1) {  //jeśli napięcie i prąd nie zmieniają się zapisz pomiary na kartę co minutę
        diff_save_sd = true;
        sd_seconds = 0;
        sd_minutes = 0;
      }
  }
  last_vol_Avg = vol_avg;  // aktualizacja do kolejnej iteracji
  //++++++++++++++++++++++++ CURRENT ++++++++++++++++++++++++++++

  cur_sampleRaw = ina219.shuntCurrent();
  cur_avg = (cur_sampleRaw * alpha) + ((last_cur_Avg) * (1 - alpha));
  current_auto = cur_avg;

  // zapis danych na karcie gdy jest ustalona różnica w pomiarach prądów

  if (!(fabs(current_auto - last_cur_Avg) <= 0.01)) {
    last_cur_Avg = cur_avg;
    diff_save_sd = true;
  }
  //+++++++++++++++++++++ FORMATOWANIE DANYCH NA LCD +++++++++++++++++++

  if (!(charger_state == off)) power_W = busvoltage * current_auto;  //ina219.busPower();

  sprintf(buf1, "%0.2f%s", busvoltage, "V");
  if (current_auto >= 1.0) sprintf(buf2, "%0.2f%s", current_auto, "A");
  else if (current_auto < 1.0) sprintf(buf2, "%0.3f%s", current_auto, "A");
  if (ampHours <= 9) sprintf(buf3, "%0.3f%s", ampHours, "Ah");
  else if ((ampHours > 9) && (ampHours < 99)) sprintf(buf3, "%0.2f%s", ampHours, "Ah");
  else if ((ampHours > 99) && (ampHours < 999)) sprintf(buf3, "%0.1f%s", ampHours, "Ah");
  else if (ampHours > 999) sprintf(buf3, "%0.0f%s", ampHours, "Ah");
  sprintf(buf4, "%0.2f%s", power_W, "W");
  if (wattHours < 0) {
  float absWh = fabs(wattHours);
  if (absWh < 10)
    sprintf(buf5, "-%0.2fWh", absWh);   // np. -9.99Wh (7 znaków)
  else if (absWh < 100)
    sprintf(buf5, "-%0.1fWh", absWh);   // np. -99.9Wh (7 znaków)
  else
    sprintf(buf5, "-%0.0fWh", absWh);   // np. -123Wh  (7 znaków)
} else {
  if (wattHours < 10)
    sprintf(buf5, "%0.3fWh", wattHours);   // np. 9.999Wh (8 znaków)
  else if (wattHours < 100)
    sprintf(buf5, "%0.2fWh", wattHours);   // np. 99.99Wh (8 znaków)
  else if (wattHours < 1000)
    sprintf(buf5, "%0.1fWh", wattHours);   // np. 999.9Wh (8 znaków)
  else
    sprintf(buf5, "%0.0fWh", wattHours);   // np. 1234Wh  (7–8 znaków)
}


}
void read_eep() {
  // Odczyt danych z pamięci flash
  Rshunt = preferences.getFloat("rshunt", 0.0f);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");

  // Debugowanie
  Serial.println(F("=== EEPROM ODCZYT ==="));
  Serial.print(F("Rshunt: "));
  Serial.println(Rshunt, 6);  // z dokładnością do 6 miejsc po przecinku

  Serial.print(F("SSID: "));
  Serial.println(ssid);

  Serial.print(F("Hasło: "));
  Serial.println(password);
  Serial.println(F("======================"));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void wyslij_speak() {

  if (THINGSPEAK == 1) {
    ThingSpeak.setField(1, buf1);
    ThingSpeak.setField(2, buf2);
    ThingSpeak.setField(3, buf3);
    //ThingSpeak.setField(4, pwm);

    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mDelay(uint16_t ms) {

  while (ms--) delay(1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void printLocalTime() {

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  time_sec = timeinfo.tm_sec;
  time_min = timeinfo.tm_min;
  time_hour = timeinfo.tm_hour;

  if (time_sec <= 9) Stime_sec = "0" + String(time_sec);
  else Stime_sec = String(time_sec);
  if (time_min <= 9) Stime_min = "0" + String(time_min);
  else Stime_min = String(time_min);
  if (time_hour <= 9) Stime_hour = "0" + String(time_hour);
  else Stime_hour = String(time_hour);
  localTime = (Stime_hour) + ":" + (Stime_min) + "." + (Stime_sec);
  sprintf(buf6, "%s", localTime);
}





///////////////////////////// ENKODER /////////////////////////////

void encoder() {

  if (rotaryEncoder.encoderChanged()) {
    if (rotaryEncoder.direction() == 1) {
      up = true;
      check_page();
    } else if (rotaryEncoder.direction() == -1) {
      down = true;
      check_page();
    }
  }
  if (rotaryEncoder.isEncoderButtonClicked()) {
    middle = true;
    check_page();
  }
}


void alarm_led(uint16_t x, byte z) {  //x - przerwa między mignięciami, z - ilość mignięć

  for (int i = 0; i < z; i += 1) {
    (digitalWrite(ERROR_LED, !(digitalRead(ERROR_LED))));
    mDelay(x);
    (digitalWrite(ERROR_LED, !(digitalRead(ERROR_LED))));
    mDelay(x);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void wifi_connect() {

  if (WIFI_FUNC) {
    if (ssid == "" || password == "") {
      Serial.println("No values saved for ssid or password");
    } else {
      WiFi.setHostname(MyHostName);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(), password.c_str());
      Serial.print("Connecting to WiFi ..");
      while (WiFi.status() != WL_CONNECTED) {
        mDelay(1000);
      }
    }
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
    Serial.println(WiFi.getHostname());
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void check_wifi_status() {

  if (WIFI_FUNC) {
    wifi_status = 0;
    if (WiFi.status() == WL_CONNECTED) {
      wifi_status = 1;
      //Serial.println("WiFi Connected ");
    } else {
      //Serial.println("WiFi not Connected ");
      WiFi.disconnect();
      //Serial.println("---------- WiFi reconnect ----------");
      WiFi.reconnect();
    }
  }
}
//////////////////////////////// Obliczanie Ah, Wh oraz czasu /////////////////////////////////////////////////

void update_minutes_and_hours(uint16_t& seconds, uint16_t& minutes, uint16_t& hours) {
  if (seconds > 59) {
    seconds = 0;
    minutes++;
  }
  if (minutes > 59) {
    minutes = 0;
    hours++;
  }
}

void calc_data_time() {

  msec = millis();
  elapsed_msec = msec - last_msec;
  elapsed_time = elapsed_msec / 1000.0;

  if (elapsed_msec >= 1000UL) {
    if (!(charger_state == off)) {
      wattSec = power_W * elapsed_time;
      wattHours = wattHours + wattSec / 3600;
      if (current_auto > 0) {
        ampSec = current_auto * elapsed_time;
        ampHours = ampHours + ampSec / 3600;
      } else if (current_auto < 0) {
        ampSec_m = current_auto * elapsed_time;
        ampHours_m = ampHours_m + ampSec_m / 3600;
      }
    }
    if (charger_state == initial)
      seconds_initial++;
    if (charger_state == bulk)
      seconds_bulk++;
    if (charger_state == absorption)
      seconds_abs++;
    if (charger_state == storage)
      seconds_storage++;
    if (charger_state == bat_float)
      seconds_float++;

    last_msec = msec;

    if (!charger_state == off) {
      ++seconds;
      ++sd_seconds;
    }
  }
  if (sd_seconds > 59) {
    ++sd_minutes;
    sd_seconds = 0;
  }
  if (seconds > 59) {
    ++minutes;
    seconds = 0;
  }
  if ((charger_state == initial) && (seconds_initial > 59)) {
    minutes_initial++;
    seconds_initial = 0;
  }
  if ((charger_state == bulk) && (seconds_bulk > 59)) {
    minutes_bulk++;
    seconds_bulk = 0;
  }
  if ((charger_state == absorption) && (seconds_abs > 59)) {
    minutes_abs++;
    minutess_abs++;
    seconds_abs = 0;
  }
  if ((charger_state == storage) && (seconds_storage > 59)) {
    minutes_storage++;
    minutess_storage++;
    seconds_storage = 0;
  }
  if ((charger_state == bat_float) && (seconds_float > 59)) {
    minutess_float++;
    minutes_float++;
    seconds_float = 0;
  }

  if (minutes > 59) {
    hours++;
    minutes = 0;
  }
  if (minutes_initial > 59) {
    hours_initial++;
    minutes_initial = 0;
  }
  if (minutes_bulk > 59) {
    hours_bulk++;
    minutes_bulk = 0;
  }
  if (minutes_abs > 59) {
    hours_abs++;
    minutes_abs = 0;
  }
  if (minutes_storage > 59) {
    hours_storage++;
    minutes_storage = 0;
  }
  if (minutes_float > 59) {
    hours_float++;
    minutes_float = 0;
  }
  //---------------------formatowanie stringów czasowych-------------------------------//

  if (hours <= 9) Shours = "0" + String(hours);
  else Shours = String(hours);
  if (minutes <= 9) Smin = "0" + String(minutes);
  else Smin = String(minutes);
  if (seconds <= 9) Ssec = "0" + String(seconds);
  else Ssec = String(seconds);
  czas = Shours + String(":") + Smin + String(":") + Ssec;

  if (charger_state == initial) {
    if (hours_initial <= 9) Shours_initial = "0" + String(hours_initial);
    else Shours_initial = String(hours_initial);
    if (minutes_initial <= 9) Smin_initial = "0" + String(minutes_initial);
    else Smin_initial = String(minutes_initial);
    if (seconds_initial <= 9) Ssec_initial = "0" + String(seconds_initial);
    else Ssec_initial = String(seconds_initial);
    czas_initial = Shours_initial + String(":") + Smin_initial + String(":") + Ssec_initial;
  }
  if (charger_state == bulk) {
    if (hours_bulk <= 9) Shours_bulk = "0" + String(hours_bulk);
    else Shours_bulk = String(hours_bulk);
    if (minutes_bulk <= 9) Smin_bulk = "0" + String(minutes_bulk);
    else Smin_bulk = String(minutes_bulk);
    if (seconds_bulk <= 9) Ssec_bulk = "0" + String(seconds_bulk);
    else Ssec_bulk = String(seconds_bulk);
    czas_bulk = Shours_bulk + String(":") + Smin_bulk + String(":") + Ssec_bulk;
  }
  if (charger_state == absorption) {
    if (hours_abs <= 9) Shours_abs = "0" + String(hours_abs);
    else Shours_abs = String(hours_abs);
    if (minutes_abs <= 9) Smin_abs = "0" + String(minutes_abs);
    else Smin_abs = String(minutes_abs);
    if (seconds_abs <= 9) Ssec_abs = "0" + String(seconds_abs);
    else Ssec_abs = String(seconds_abs);
    czas_abs = Shours_abs + String(":") + Smin_abs + String(":") + Ssec_abs;
  }
  if (charger_state == storage) {
    if (hours_storage <= 9) Shours_storage = "0" + String(hours_storage);
    else Shours_storage = String(hours_storage);
    if (minutes_storage <= 9) Smin_storage = "0" + String(minutes_storage);
    else Smin_storage = String(minutes_storage);
    if (seconds_storage <= 9) Ssec_storage = "0" + String(seconds_storage);
    else Ssec_storage = String(seconds_storage);
    czas_storage = Shours_storage + String(":") + Smin_storage + String(":") + Ssec_storage;
  }
  if (charger_state == bat_float) {
    if (hours_float <= 9) Shours_float = "0" + String(hours_float);
    else Shours_float = String(hours_float);
    if (minutes_float <= 9) Smin_float = "0" + String(minutes_float);
    else Smin_float = String(minutes_float);
    if (seconds_float <= 9) Ssec_float = "0" + String(seconds_float);
    else Ssec_float = String(seconds_float);
    czas_float = Shours_float + String(":") + Smin_float + String(":") + Ssec_float;
  }
}
////////////////////////////////- strona aktualizacji OTA -////////////////////////////////////////////

const char* loginIndex =
  "<form name='loginForm'>"
  "<table width='20%' bgcolor='A09F9F' align='center'>"
  "<tr>"
  "<td colspan=2>"
  "<center><font size=4><b>ESP32 Login Page</b></font></center>"
  "<br>"
  "</td>"
  "<br>"
  "<br>"
  "</tr>"
  "<tr>"
  "<td>Username:</td>"
  "<td><input type='text' size=25 name='userid'><br></td>"
  "</tr>"
  "<br>"
  "<br>"
  "<tr>"
  "<td>Password:</td>"
  "<td><input type='Password' size=25 name='pwd'><br></td>"
  "<br>"
  "<br>"
  "</tr>"
  "<tr>"
  "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
  "</tr>"
  "</table>"
  "</form>"
  "<script>"
  "function check(form)"
  "{"
  "if(form.userid.value=='admin' && form.pwd.value=='admin')"
  "{"
  "window.open('/serverIndex')"
  "}"
  "else"
  "{"
  " alert('error Password or Username')/*displays error message*/"
  "}"
  "}"
  "</script>";

/*
   Server Index Page
*/

const char* serverIndex =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Update'>"
  "</form>"
  "<div id='prg'>progress: 0%</div>"
  "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
  "},"
  "error: function (a, b, c) {"
  "}"
  "});"
  "});"
  "</script>";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mdns() {

  if (WIFI_FUNC) {
    /*use mdns for host name resolution*/
    if (!MDNS.begin(host)) {  //http://esp32.local
      Serial.println("Error setting up MDNS responder!");
      while (1) {
        delay(1000);
      }
    }
    Serial.println("mDNS responder started");
    /*return index page which is stored in serverIndex */
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", loginIndex);
    });
    server.on("/serverIndex", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex);
    });
    /*handling uploading firmware file */
    server.on(
      "/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
      },
      []() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
          Serial.printf("Update: %s\n", upload.filename.c_str());
          if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  //start with max available size
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          /* flashing firmware to ESP*/
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) {  //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          } else {
            Update.printError(Serial);
          }
        }
      });
  }
}

void print_serial(void) {

  // Serial.print("charging_type -");
  // Serial.print(charging_type);
  // Serial.print(" battery capacity -   ");
  // Serial.print(battery[battery_type].capacity);
  // Serial.println(" Ah ");

  // Serial.print("busvoltage: ");
  // Serial.println(busvoltage);
  // Serial.print("u_bulk: ");
  // Serial.println(battery[battery_type].u_bulk);
  // Serial.print("minutess_storage: ");
  // Serial.println(minutess_storage);
  // Serial.print("seconds_storage: ");
  // Serial.println(seconds_storage);
  // Serial.print("TIME_STORAGE: ");
  // Serial.println(TIME_STORAGE);

  // Serial.print("output ");
  // Serial.print(output);
  // Serial.print("   PID Value / ");
  // Serial.print(pidValue);
  // Serial.print("   ");
  // Serial.print("PID_error / ");
  // Serial.print(PID_error);
  // Serial.print("   ");
  // Serial.print("PID_errSum / ");
  // Serial.print(PID_errSum);
  // Serial.print("   ");
  // Serial.print("PID_dErr / ");
  // Serial.print(PID_dErr);
  // Serial.println("   ");
  //Serial.println(sd_seconds);

  // if (charger_state == absorption) {
  //   Serial.print(" -minutess_abs =");
  //    Serial.print(minutess_abs);
  //   Serial.print(":");
  //   Serial.print(seconds_abs);
  //   Serial.println("  ");
  //  } else if (charger_state == storage) {
  //   Serial.print("- minutess_storage =");
  //   Serial.print(minutess_storage);
  //   Serial.print("- seconds_storage -");
  //   Serial.print(seconds_storage);
  //   Serial.println("  ");
  // } else if (charger_state == bat_float) {
  //   Serial.print("- minutess_float =");
  //   Serial.print(minutess_float);
  //   Serial.print("- seconds_float -");
  //   Serial.print(seconds_float);
  //Serial.println("  ");
  // }


  // Serial.print(" topItem = ");
  // Serial.println(topItem);
  // Serial.print("-- napięcie = ");
  // Serial.println(busvoltage);


  Serial.print("- Page -");
  Serial.print(page);
  Serial.print("- Menuitem -");
  Serial.print(menuitem);
  // Serial.print("- DischarStart -");
  // Serial.print(DischarStart);
  // Serial.print("- DischarSet -");
  // Serial.print(DischarSet);
  // Serial.print("- CurrentDisch -");
  // Serial.print(CurrentDisch);
 Serial.print("- asym_mode -");
  Serial.print(asym_mode);



  // Serial.print("- menuitem_stop =");
  // Serial.println(menuitem_stop);
  // Serial.print("- battery_type =");
  // Serial.print(battery_type);
  Serial.print("- charging_end -");
  Serial.print(charging_end);
  Serial.println("  ");
  /*
  


    Serial.print("- Page -");
    Serial.print(page);
    Serial.print("- sd_interval -");
    Serial.print(sd_interval);
    Serial.println(" ");
    /* Serial.print("minutess_abs");
    Serial.print(" ");
    Serial.print(minutess_abs);
    Serial.print(" ");
    Serial.print("current_1h");
    Serial.print(" ");
    Serial.print(batt_abs_current_1h);
    Serial.print(" ");
    Serial.print("current_3h");
    Serial.print(" ");
    Serial.print(batt_abs_current_3h);
    Serial.print(" ");
    Serial.print("current_5h");
    Serial.print(" ");
    Serial.print(batt_abs_current_5h);
    Serial.print(" ");
    Serial.print("current_7h");
    Serial.print(" ");
    Serial.print(batt_abs_current_7h);
    Serial.println(" ");



    


    Serial.print(" battery[0].u_bulk - ");
    Serial.print(battery[0].u_bulk);
    Serial.print(" battery[1].u_bulk - ");
    Serial.print(battery[1].u_bulk);
    Serial.print(" battery[2].u_bulk - ");
    Serial.print(battery[2].u_bulk);
    Serial.print(" battery[3].u_bulk - ");
    Serial.print(battery[3].u_bulk);
    Serial.print(" battery[1].pojemnosc - ");
    Serial.print(battery[1].capacity);
    Serial.print(" battery[type].pojemnosc - ");
    Serial.print(battery[battery_type].capacity);
    Serial.print("- Pojemn =");
    Serial.print(pojemnosc);
    Serial.println("  ");

    Serial.print("-- a_diff= ");
    Serial.print(voltage_diff);
    Serial.print("-- b_diff= ");
    Serial.print(current_diff);
    Serial.print("-- diff --");
    Serial.print(diff_save_sd);
    Serial.println("   ");
      /
    /*
       Serial.println("  ");
      Serial.print(" move  ");
      Serial.print(rotaryEncoder.direction());
      Serial.print(" value  ");
      Serial.println(rotaryEncoder.readEncoder());
      Serial.println("button pressed");
      Serial.print("cur_sample - ");
      Serial.print(cur_sample);
      Serial.print(" ");
      Serial.print("last_cur_Avg ");
      Serial.print(last_cur_Avg);
      Serial.println("  ");

      Serial.print("odczytany ssid z sd: ");
      Serial.println(readssid);
      Serial.print("odczytany pass z sd: ");
      Serial.println(readpass);

  */
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////