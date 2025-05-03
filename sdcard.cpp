

#include "SD.h"
#include "FS.h"
#include <SPI.h>
#include <Arduino.h>
#include <Preferences.h>

//-----------------------------------------------------------------------------------------------------------------------//
#include "sdcard.h"

#include "display.h"
#include "check_page.h"
#include "common.h"


//-----------------------------------------------------------------------------------------------------------------------//

int n = 0;  // nr pliku na sd

void sd_check() {  // initialialize SD card

  SD.begin(SDchipSelect);
  if (!SD.begin(SDchipSelect)) {
    //alarm_led(200, 8);
    if (DEBUG_SD) {
      Serial.println("Card Mount Failed");
      //return;
    }
  }

  //++++++++++++++++++++++++++++++++++++++++++ nagłówek pliku pid ++++++++++++++++++++++++++++++++++++++++++++++

  snprintf(pid_filename, sizeof(pid_filename), "/pid_log%03d.csv", n);  // przygotowanie nazwy pliku log
  while (SD.exists(pid_filename)) {
    n++;
    snprintf(pid_filename, sizeof(pid_filename), "/pid_log%03d.csv", n);  // jeśli plik istnieje zmień nazwę
  }
  String PID_String = "";
  PID_String += "kp=" + String(kp) + ";" + "ki=" + String(ki) + ";" + "kd=" + String(kd) + "\r\n" + "\r\n";
  writeFile(SD, pid_filename, PID_String.c_str());                          // zapisz nowy plik na sd
  appendFile(SD, pid_filename, "Czas; P; I; D; Output;Napiecie; \r\n");  // dopisz dane na sd

  //++++++++++++++++++++++++++++++++++++++++++ nagłówek pliku datalog ++++++++++++++++++++++++++++++++++++++++++++++

  snprintf(filename, sizeof(filename), "/datalog%03d.csv", n);  // przygotowanie nazwy pliku log
  while (SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "/datalog%03d.csv", n);  // jeśli plik istnieje zmień nazwę
  }
  writeFile(SD, filename, "Czas; Napiecie; Prad; Ah; W; Wh \r\n");  // zapisz nowy plik na sd

  //++++++++++++++++++++++++++++++++++++++++++ nagłówek pliku stat_log ++++++++++++++++++++++++++++++++++++++++++++++++++++

  snprintf(stats_filename, sizeof(stats_filename), "/stat_log%03d.csv", n);  // przygotowanie nazwy pliku log
  while (SD.exists(stats_filename)) {                                        // jeśli plik istnieje zmień nazwę
    n++;
    snprintf(stats_filename, sizeof(stats_filename), "/stat_log%03d.csv", n);
  }
  writeFile(SD, stats_filename, "Firmware ver.");  // zapisz nowy plik na sdCOMPILATION
  appendFile(SD, stats_filename, COMPILATION);
  appendFile(SD, stats_filename, "\r\n");
  appendFile(SD, stats_filename, "Charging stats for ");  // zapisz nowy plik na sd
  appendFile(SD, stats_filename, stats_filename);
  appendFile(SD, stats_filename, "\r\n");
  // appendFile(SD, stats_filename, "----------------------------------- \r\n");

  //+++++++++++++++++++++++++++++++++++++++++ nagłówek pliku abs_log +++++++++++++++++++++++++++++++++++++++++++++++++++

  snprintf(current_filename, sizeof(current_filename), "/abs_log%03d.csv", n);  // przygotowanie nazwy pliku log
  while (SD.exists(current_filename)) {                                         // jeśli plik istnieje zmień nazwę
    n++;
    snprintf(current_filename, sizeof(current_filename), "/abs_log%03d.csv", n);
  }
  writeFile(SD, current_filename, "Current readings every hour in absorption mode. ");  // zapisz nowy plik na sd
  appendFile(SD, current_filename, "Filename: ");
  appendFile(SD, current_filename, current_filename);
  appendFile(SD, current_filename, "\r\n");
  appendFile(SD, current_filename, "------------------------------------------------- \r\n");

  //+++++++++++++++++++++++++++++++++++++++++ odczyt danych do Wifi z karty SD ++++++++++++++++++++++++++++++++++++++++++++++++

  if (SD.exists(ssidfilename)) {
    readssid = readFile(SD, "/_ssid.txt");  // odczyt pliku ssid.txt(uwaga na spacje w nazwie)
    readpass = readFile(SD, "/_pass.txt");  // odczyt pliku pass.txt(uwaga na spacje w nazwie)

    preferences.putString("ssid", readssid);      // zapis ssid do flash
    preferences.putString("password", readpass);  // zapis pass do flash
  }
}

///////////////////////////////// WARUNKI DO ZAPISU NA SD ////////////////////////////////////

void pid_log(double _p, double _i, double _d, uint16_t _pidValue) {

  if (charger_state == absorption) {
    String dataString = "";
    dataString += czas + ";" + String(_p) + ";" + String(_i) + ";" + String(_d) + ";" + String(_pidValue) + ";" + String(busvoltage) + "\r\n";
    dataString.replace('.', ',');
    appendFile(SD, pid_filename, dataString.c_str());
    //Serial.println("pid_log_saved");
  }
}


void logger_time(void) {

  if (charging_end == false) pid_log(P, I, D, pidValue);

  if (
     (charging_end == false && diff_save_sd == true) ||
     (auto_save_sd == false) ||
     (force_sd == true) ||
     (DischarStart && diff_save_sd == true)
   )
 {  // "jeżeli trwa ładowanie i jest różnica lub automatyczny zapis na sd jest wył"
    force_sd = false;
    String dataString = "";
    dataString += czas + ";" + String(busvoltage) + ";" + String(current_auto) + ";" + String(ampHours) + ";" + String(power_W) + ";" + String(wattHours) + "\r\n";
    dataString.replace('.', ',');
    if (DEBUG_SD) {
     //Serial.println("datalog_saved");
    }
    appendFile(SD, filename, dataString.c_str());
    diff_save_sd = false;
  }
}

//////////////////////////////////// ZAPIS STATYSTYK ///////////////////////////////////////////

void logLine(String text) {
  appendFile(SD, stats_filename, text.c_str());
}

void stats_log() {
  logLine(" Initial time - " + czas_initial + "\r\n");
  logLine(" Initial Ah - " + String(initial_Ah) + "\r\n");
  
  logLine(" Bulk time - " + czas_bulk + "\r\n");
  logLine(" Bulk Ah - " + String(bulk_Ah) + "\r\n");

  logLine(" Absorption time - " + czas_abs + "\r\n");
  logLine(" Absorption Ah - " + String(abs_Ah) + "\r\n");

  logLine(" Storage time - " + czas_storage + "\r\n");
  logLine(" Storage Ah - " + String(storage_Ah) + "\r\n");

  logLine(" Float time - " + czas_float + "\r\n");
  logLine(" Float Ah - " + String(float_Ah) + "\r\n");

  logLine("\r\n");
  logLine(" Total charging time " + czas + "\r\n\r\n");

  logLine(" Total Ah - " + String(ampHours, 3) + "Ah\r\n");
  logLine(" Total -Ah - " + String(ampHours_m, 3) + "Ah\r\n");
  logLine(" Total Wh - " + String(wattHours, 3) + "Wh\r\n");
  logLine(" Total SD rows - " + String(sd_num) + "\r\n");

  logLine(" AutomaticEndOfCharging - " + String(AutomaticEndOfCharging ? "true" : "false") + "\r\n");
  logLine("----------------------------------- \r\n");

  Serial.println("stats_log_saved");
}

//////////////////////////////////// DOPISYWANIE DANYCH DO PLIKU //////////////////////////////////////////////

void appendFile(fs::FS& fs, const char* path, const char* message) {  // appendFile nie nadpisuje poprzedniej zawartości
  if (DEBUG_SD) {
    //Serial.printf("Appending to file: %s\n", path);
  }

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    if (DEBUG_SD) {
      Serial.println("Failed to open file for appending");
      alarm_led(200, 3);
    }
  }
  if (file.print(message)) {
    save_to_sd = true;
    sd_num++;  // numer zapisu na sd
    if (DEBUG_SD) {
      //Serial.println("Message appended");
    }
  } else {
    if (DEBUG_SD) {
      Serial.println("Append failed");
      alarm_led(200, 5);
    }
  }
  file.close();
}

//////////////////////////////////////// ZAPIS DO PLIKU ////////////////////////////////////////////

void writeFile(fs::FS& fs, const char* path, const char* message) {  // writeFile nadpisuje poprzednią zawartość
  //save_to_sd = false;
  if (DEBUG_SD) {
    Serial.printf("Writing file: %s\n", path);
  }

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    if (DEBUG_SD) {
      Serial.println("Failed to open file for writing");
    }
    return;
  }
  if (file.print(message)) {
    //save_to_sd = true;
    if (DEBUG_SD) {
      Serial.println("File written");
    }
  } else {
    if (DEBUG_SD) {
      Serial.println("Write failed");
      alarm_led(200, 10);
    }
  }
  file.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

String readFile(fs::FS& fs, const char* path) {

  String buffer_read = "";
  char character;
  uint8_t totalBytes = 0;

  if (DEBUG_SD) {
    Serial.printf("Reading file: %s\n", path);
  }
  File file = fs.open(path);
  if (!file) {
    if (DEBUG_SD) {
      Serial.println("Failed to open file for reading");
      return "";
    }
  }
  totalBytes = file.size();

  if (DEBUG_SD) {
    Serial.print("file size: ");
    Serial.print(totalBytes);
    Serial.println(" Bytes");
  }

  while (file.available()) {
    for (int i = 0; i < totalBytes; i++) {
      character = file.read();
      buffer_read = buffer_read + character;
    }
    if (DEBUG_SD) {
      Serial.print("buffer_read ");
      Serial.println(buffer_read);
    }
    return buffer_read;
  }
  file.close();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
