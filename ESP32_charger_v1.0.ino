//  NOWE MENU PAGE 14
/*
  Szkic używa 945169 bajtów (72%) pamięci programu. Zmienne globalne używają 52380 bajtów. 01.05.2025
  usprawnienia do zrobienia: uruchomienie bez wifi, debug z menu, recond z wykrywaniem naładowania, zapis nastaw do flash,
  struktury czasowe, show pwm, show sd_counter, 
  
*/
// w trybie abs zapis co 1 min.


#include <Arduino.h>

#include <Timers.h>  // dołączona biblioteka Timers do obsługi wątków
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <U8g2lib.h>
#include <INA219.h>
#include "AiEsp32RotaryEncoder.h"
#include <Preferences.h>
#include "ThingSpeak.h"
//-----------------------------------------------------------------------------------------------------------------------//

#include "display.h"
#include "check_page.h"
#include "common.h"
#include "sdcard.h"
#include "charger.h"

//-----------------------------------------------------------------------------------------------------------------------//
WebServer server;
Preferences preferences;
File myFile;
WiFiClient client;

struct tm timeinfo;  //struktura do odczytu czasu z internetu

extern TCHARGER_STATE charger_state;
extern TBATTERY_TYPE battery_type;

volatile int8_t lastMovementDirection = 0;
volatile unsigned long lastMovementAt = 0;

void IRAM_ATTR readEncoderISR() {
  rotaryEncoder.readEncoder_ISR();
}

U8G2_ST7567_ENH_DG128064I_F_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);
INA219 ina219;

Timers<8> akcja;

/////////////////////////////////////////////////////////

void setup() {

  Serial.begin(115200);
  if (SerialPort) Serial.println("Serial port ready");
  preferences.begin("credentials", false);
  sd_check();
  read_eep();  // odczyt rShunt, ssid oraz pass z flash

  if (WIFI_FUNC) {
    wifi_connect();
    ThingSpeak.begin(client);
    mdns();
    server.begin();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
  }

  Wire.setClock(400000);
  Wire.begin();
  u8g2.setI2CAddress(0x3F * 2);
  u8g2.begin();
  

  ledcAttachPin(CHARGE_PIN, pwm_charge_channel);                      // attach the channel to the GPIO to be controlled
  ledcSetup(pwm_charge_channel, charging_freq, charging_resolution);  // configure PWM functionalitites

  ledcAttachPin(DISCHARGE_PIN, pwm_discharge_channel);                         // attach the channel to the GPIO to be controlled
  ledcSetup(pwm_discharge_channel, discharging_freq, discharging_resolution);  // configure PWM functionalitites

  pinMode(ERROR_LED, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH);

  ina219.begin();
  ina219.configure(INA219::RANGE_32V, INA219::GAIN_8_320MV, INA219::ADC_128SAMP, INA219::ADC_128SAMP, INA219::CONT_SH_BUS);
  ina219.calibrate(Rshunt, V_SHUNT_MAX, V_BUS_MAX, I_MAX_EXPECTED);

  pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);  // czy musi być pullup?
  pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(0, 5000, false);  //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoder.disableAcceleration();
  rotaryEncoder.setEncoderValue(1000);

  akcja.attach(0, 79, read_data);
  akcja.attach(1, 251, drawMenu);
  akcja.attach(2, 1033, logger_time);
  akcja.attach(3, 1009, printLocalTime);
  akcja.attach(4, 113, run_charger);
  akcja.attach(5, 100039, check_wifi_status);
  akcja.attach(6, 30011, wyslij_speak);
  akcja.attach(7, 997, print_serial);

  //errSum = 0;
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  akcja.process();
  calc_data_time();  //obliczanie Ah, Wh oraz czasu
  encoder();
  akcja.updateInterval(2, sd_interval);  // zmiana interwału zapisu na sd z menu
  if (charger_state == absorption || charger_state == storage || charger_state == bat_float) {
    akcja.updateInterval(2, 5000);
}
  if (WIFI_FUNC) {
    server.handleClient();
  }
  if (init_param) bat_init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
