
#include <Arduino.h>
#include <stdlib.h>

#include "display.h"
#include "check_page.h"
#include "common.h"
#include "sdcard.h"
#include "charger.h"
#include <string.h>

//uint16_t intervals_min[26] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 }; // do testów
uint16_t intervals_min[26] = { 30, 60, 90, 120, 180, 240, 300, 360, 420, 480, 540, 600, 660, 720, 780, 840, 900, 960, 1020, 1080, 1140, 1200, 1260, 1320, 1380, 1440 };
uint16_t intervals_h[18] = { 60, 120, 180, 240, 300, 360, 450, 540, 630, 720, 810, 900, 990, 1080, 1170, 1260, 1350, 1440 };
uint16_t intervals[26] = {};
uint16_t intervals_size = 18;

uint8_t pwm_memory = 2;  // Pamięć ostatniego stabilnego PWM (globalnie)
void handle_bulk_mode();

bool save_flags[26] = { false };
bool asym_ch = true;
bool asym_disch = false;
bool stor_alw_on = false;
bool float_alw_on = false;

String batt_abs_current[26] = {};
String abs_current = " ";
bool AutomaticEndOfCharging = false;
ChargingType charging_type = normal_charging;

float li_battery_type = 1.25;
uint8_t load_div = 50;

float li_ion_currentOff = 0.125;
float last_capacity_disch;
float last_capacity_ch;
float elapsed_capacity_ch;
float cur_capacity_ch;
float prev_capacity_ch, prev_capacity_disch;
float li_normal_mode;
float li_fast_mode;

void handle_initial_mode() {
  initial_Ah = ampHours;

  if (charging_type == fast_charging) {
    charger_state = bulk;
    return;
  }

  if (!asym_mode) {
    if (busvoltage > MIN_BAT_VOLTS) {
      float target_current = battery[battery_type].capacity / initial_charging;
      if (current_auto > (target_current + 0.01)) {
        pwm--;
        charge_pwm_duty();
      } else if (current_auto < (target_current - 0.01)) {
        pwm++;
        charge_pwm_duty();
      }
    } else {
      charger_state = off;
    }
  }

  else if (asym_mode) {  // asym_mode == true
    handle_asym_mode();
  }

  if ((busvoltage >= NOM_BAT_VOLTS) || battery_type == battery_li_2s || battery_type == battery_li_3s) {
    Serial.println("+++++++++ bulk +++++++++");
    charger_state = bulk;
  }
}

void handle_asym_mode() {

  float capacity = battery[battery_type].capacity;              // Pojemność akumulatora
  float charge_target = capacity / 1000.0;                      // 1/1000 pojemności do ładowania
  float discharge_target = charge_target * (asym_val / 100.0);  // % rozładowania względem poprzedniego ładowania

  if (asym_ch) {  // ładowanie
    float target_current = capacity / charging_type;

    if (current_auto > (target_current + 0.01)) {
      pwm--;
      charge_pwm_duty();
    } else if (current_auto < (target_current - 0.01)) {
      pwm++;
      charge_pwm_duty();
    }

    if ((ampHours - prev_capacity_ch) >= charge_target) {
      prev_capacity_disch = ampHours_m;
      asym_ch = false;
      pwm_memory = pwm;  // ZAPAMIĘTAJ PWM!
      pwm = 0;
      charge_pwm_duty();
    }
  }

  else {  // rozładowanie
    discharge_pwm_duty(PWM_MAX);
    if ((abs(ampHours_m) + prev_capacity_disch) >= discharge_target) {
      prev_capacity_ch = ampHours;
      asym_ch = true;
      discharge_pwm_duty(PWM_MIN);
    }
  }

  last_capacity_ch = ampHours;
  last_capacity_disch = ampHours_m;
 
  if (busvoltage >= NOM_BAT_VOLTS) {
    asym_ch = true;
    //handle_bulk_mode();
    charger_state = bulk;
    Serial.println("=> Opuszczono tryb asymetryczny");
    return;
  }
}
//*****************************************************************************************************

void handle_bulk_mode() {
  bulk_Ah = ampHours - initial_Ah;

  // Przejście do absorption
  if (busvoltage > battery[battery_type].u_bulk) {
    pwm = 0;
    charge_pwm_duty();
    force_sd = true;
    logger_time();  // zapis do datalog.csv
    String abs_current = String("0") + "h = " + String(current_auto);
    appendFile(SD, current_filename, abs_current.c_str());
    appendFile(SD, current_filename, "\r\n");
    mDelay(1000);
    Serial.println("+++++++++ absorption +++++++++");
    charger_state = absorption;
    return;
  }

  // Sterowanie PWM w zależności od typu baterii
  if (battery_type == battery_li_2s || battery_type == battery_li_3s) {
    handle_bulk_lipo();
  } else {
    handle_bulk_standard();
  }
}

//*****************************************************************************************************

void handle_bulk_standard() {
  float target_current = battery[battery_type].capacity / charging_type;

  if (current_auto > (target_current + 0.01)) {
    pwm--;
    charge_pwm_duty();
  } else if (current_auto < target_current) {
    mDelay(250);
    pwm++;
    charge_pwm_duty();
  }
}

//*****************************************************************************************************

void handle_bulk_lipo() {
  float threshold_base = li_battery_type;

  if (charging_type == normal_charging) {
    if (current_auto > threshold_base + 0.25) {
      pwm--;
      charge_pwm_duty();
    } else if (current_auto < threshold_base + 0.2) {
      mDelay(250);
      pwm++;
      charge_pwm_duty();
    }
  }

  else if (charging_type == fast_charging) {
    if (current_auto > 2 * (threshold_base + 0.25)) {
      pwm--;
      charge_pwm_duty();
    } else if (current_auto < 2 * (threshold_base + 0.2)) {
      mDelay(250);
      pwm++;
      charge_pwm_duty();
    }
  }
}

//*****************************************************************************************************

void handle_absorption_mode() {
  abs_Ah = ampHours - bulk_Ah - initial_Ah;

  // Regulacja PWM przez PID
  if (PID) pid_pwm_duty();

  // 1. Zapis prądu co określony interwał
  log_absorption_currents();

  // 2. Sprawdzenie warunku zakończenia trybu absorption
  if (check_absorption_end_conditions()) {
    return;
  }

  // 3. Specjalne zakończenie dla Li-Ion
  if (check_absorption_li_ion_end()) {
    return;
  }
}

//*****************************************************************************************************

void log_absorption_currents() {
  for (uint8_t i = 0; i < intervals_size; i++) {
    if (!save_flags[i] && minutess_abs == intervals[i]) {
      batt_abs_current[i] = String(current_auto, 2);
      String abs_current = String(intervals[i]) + "min = " + batt_abs_current[i] + "A";

      Serial.printf("i=%d batt_abs_current i=%sA\n", i, batt_abs_current[i].c_str());
      appendFile(SD, current_filename, abs_current.c_str());
      appendFile(SD, current_filename, "\r\n");
      save_flags[i] = true;
    }
  }
}

//*****************************************************************************************************

bool check_absorption_end_conditions() {
  for (uint8_t i = 2; i < intervals_size; i++) {
    if ((battery_type != battery_li_2s && battery_type != battery_li_3s) && (minutess_abs == intervals[i]) && (seconds_abs <= 1)) {

      float curr_now = batt_abs_current[i].toFloat();
      float curr_past = batt_abs_current[i - 2].toFloat();

      if ((curr_now >= curr_past) || (current_auto <= battery[battery_type].capacity / load_div)) {
        Serial.printf("batt_abs_current[i]=%.2f, batt_abs_current[i-2]=%.2f\n", curr_now, curr_past);
        Serial.println("case 0");
        appendFile(SD, current_filename, "Case 0: Current greater or equal ");
        pwm = 0;
        charge_pwm_duty();
        force_sd = true;
        logger_time();
        Serial.println("+++++++++ storage +++++++++");
        charger_state = storage;
        return true;
      }
    }
  }
  return false;
}

//*****************************************************************************************************

bool check_absorption_li_ion_end() {
  if ((battery_type == battery_li_2s || battery_type == battery_li_3s) && current_auto <= li_ion_currentOff) {

    pwm = 0;
    charge_pwm_duty();
    Serial.println("case 0");
    appendFile(SD, current_filename, "Case 0: Current less or equal ");
    force_sd = true;
    logger_time();
    stats_log();
    Serial.println("+++++++++ charging end +++++++++");
    charging_end = true;
    charger_state = off;
    return true;
  }
  return false;
}

//*****************************************************************************************************

void handle_storage_mode() {
  storage_Ah = ampHours - abs_Ah - bulk_Ah - initial_Ah;

  if (PID) pid_pwm_duty();  // PID regulacja napięcia/prądu

  if ((busvoltage > battery[battery_type].u_bulk) || (TIME_STORAGE <= minutess_storage)) {
    pwm = 0;
    charge_pwm_duty();
    force_sd = true;
    logger_time();
    Serial.println("Storage end");
    charger_state = bat_float;
  }
}

//*****************************************************************************************************

void handle_bat_float_mode() {
  float_Ah = ampHours - storage_Ah - abs_Ah - bulk_Ah - initial_Ah;

  // Jeśli tryb float nie jest ustawiony na działanie ciągłe
  if (!float_alw_on) {
    // Sprawdź warunki zakończenia float
    if ((busvoltage > battery[battery_type].u_bulk) || (TIME_FLOAT <= minutess_float)) {
      pwm = 0;
      charge_pwm_duty();
      AutomaticEndOfCharging = true;
      stats_log();
      force_sd = true;
      logger_time();
      Serial.println("Charging end");
      charging_end = true;
      charger_state = off;
    }
  }
}

//*****************************************************************************************************

void reset_charger_state() {
  charging_end = true;
  DischarSet = false;
  DischarStart = false;
  pwm = 0;
  pidValue = 0;
  charge_pwm_duty();
  discharge_pwm_duty(0);
  charger_state = off;
}


void handle_off_mode() {

  // jeśli ładowarka jest bezczynna
  if (charging_end || !DischarSet || !DischarStart) {
    pwm = 0;
    pidValue = 0;
    charge_pwm_duty();
    discharge_pwm_duty(0);
  }
  // Jeśli ładowanie zostało zakończone lub nie rozpoczęte
  if (!charging_end) {
    // Dla Li-ion 3S
    if (battery_type == battery_li_3s) {
      if (busvoltage >= min_voltage_li_3s)
        charger_state = bulk;
    }
    // Dla Li-ion 2S
    else if (battery_type == battery_li_2s) {
      if (busvoltage >= min_voltage_li_2s)
        charger_state = bulk;
    }
    // Dla innych typów baterii
    else if (battery_type == battery_std || battery_type == battery_agm || battery_type == battery_caca || battery_type == battery_gel) {
      if (busvoltage < BULK_BAT_VOLTS)
        charger_state = initial;
      else {
        Serial.println("+++++++++ bulk +++++++++");
        charger_state = bulk;
      }
    }
  }

  else if (charging_end && DischarSet && DischarStart) {
    charger_state = bat_disch;
  }
}

//*****************************************************************************************************

void handle_bat_disch_mode() {
  if (charging_end && DischarSet && DischarStart) {
    // Kontrola prądu rozładowania
    if (fabs(current_auto) > CurrentDisch + 0.05) {
      pwm--;
    } else if (fabs(current_auto) < CurrentDisch) {
      pwm++;
    }

    // Ograniczenie wartości PWM do zakresu 0–512
    if (pwm < 0) pwm = 0;
    if (pwm > 512) pwm = 512;

    // Przejście w tryb off, jeśli osiągnięto napięcie końcowe
    if (busvoltage <= dischVoltage + 0.01) {
      Serial.println("Rozładowanie zakończone – przejście do OFF");
      DischarSet = false;     // <--- ZATRZYMA powrót do bat_disch
      DischarStart = false;   // <--- ZATRZYMA powrót do bat_disch
      reset_charger_state();  // charger_state = off, PWM = 0, etc.
      AutomaticEndOfCharging = true;
      stats_log();
      return;
    }

    // Ustawienie PWM rozładowania
    discharge_pwm_duty(pwm);
  } else charger_state = off;
}

//*****************************************************************************************************

void run_charger(void) {
  switch (charger_state) {  // Sprawdzamy stan ładowarki
    case off:
      handle_off_mode();  // Wywołanie odpowiedniej funkcji dla trybu "off"
      break;

    case initial:
      handle_initial_mode();  // Wywołanie odpowiedniej funkcji dla trybu "initial"
      break;

    case bulk:
      handle_bulk_mode();  // Wywołanie odpowiedniej funkcji dla trybu "bulk"
      break;

    case absorption:
      handle_absorption_mode();  // Wywołanie odpowiedniej funkcji dla trybu "absorption"
      break;

    case storage:
      handle_storage_mode();  // Wywołanie odpowiedniej funkcji dla trybu "storage"
      break;

    case bat_float:
      handle_bat_float_mode();  // Wywołanie odpowiedniej funkcji dla trybu "bat_float"
      break;

    case bat_disch:
      handle_bat_disch_mode();  // Wywołanie odpowiedniej funkcji dla trybu "bat_disch"
      break;

    default:
      // Obsługuje sytuację, gdy stan ładowarki nie jest rozpoznany
      Serial.println("Unknown charger state!");
      break;
  }
}


void pid_pwm_duty() {

  ledcWrite(pwm_charge_channel, pidValue);
  mDelay(1);
}

void charge_pwm_duty() {

  if ((charging_end == false) || (pwm_auto == false)) {
    if (pwm > PWM_MAX) {
      pwm = PWM_MAX;
    }
    if (pwm < PWM_MIN) {
      pwm = 0;
    }
    if (pwm < PWM_MAX) {
      ledcWrite(pwm_charge_channel, pwm);

    } else if (pwm == PWM_MAX) {
      ledcWrite(pwm_charge_channel, PWM_MAX - 1);
    }
  }

  if ((charging_end == true) && (pwm_auto == true))
    ledcWrite(pwm_charge_channel, PWM_MIN);

  //}

  // else if (charger_state == absorption) {
  //   ledcWrite(pwm_charge_channel, pidValue);
  //   delayMicroseconds(100);
  // }
}
void discharge_pwm_duty(uint16_t y) {

  ledcWrite(pwm_discharge_channel, y);
  delayMicroseconds(100);
}
