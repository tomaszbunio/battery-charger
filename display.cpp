

#include <Arduino.h>
#include <U8g2lib.h>
#include <INA219.h>


//-----------------------------------------------------------------------------------------------------------------------//

#include "display.h"
#include "check_page.h"
#include "common.h"
#include "sdcard.h"

//-----------------------------------------------------------------------------------------------------------------------//

#define R_SHUNT 0.11
#define V_SHUNT_MAX 0.32
#define V_BUS_MAX 32
#define I_MAX_EXPECTED 3.2

extern TCHARGER_STATE charger_state;
extern TBATTERY_TYPE battery_type;
extern TBATT battery[7];

void drawMenu() {

  if (page == 0) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont15_tf);
    u8g2.drawStr(50, 40, buf1);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    if (WIFI_FUNC) {
      u8g2.setCursor(80, 10);
      u8g2.print(buf6);  //localTime
    }
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(1, 10, buf2);
    u8g2.setCursor(85, 20);
    u8g2.print(czas);
    u8g2.drawStr(1, 20, buf3);
    u8g2.setCursor(50, 20);
    u8g2.print(ampHours_m, 3);

    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(50, 30);
    if ((charger_state == initial) || (charger_state == bulk) || (charger_state == bat_disch)) {
      u8g2.print("pwm-");
      u8g2.print(pwm);
    } else if((charger_state == absorption) || (charger_state == storage) || (charger_state == bat_float)){
      u8g2.print("pid-");
      u8g2.print(pidValue);
    }
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(100, 30);
    if (pwm_auto == false) u8g2.print("PS");
    else if (charger_state == off) u8g2.print("Off  ");
    else if (charger_state == bulk) u8g2.print("Bulk ");
    else if (charger_state == initial) u8g2.print("Init ");
    else if (charger_state == absorption) u8g2.print("Abs ");
    else if (charger_state == bat_float) u8g2.print("Float");
    else if (charger_state == bat_disch) {
      u8g2.setCursor(94, 30);
      u8g2.print("Disch");
    }
    else if (charger_state == storage) {
      u8g2.setCursor(87, 30);
      u8g2.print("Storage");
    }

    u8g2.drawStr(1, 30, buf4);
    u8g2.drawStr(1, 40, buf5);
    u8g2.setCursor(100, 40);
    u8g2.print(sd_num);

    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(1, 63);
    u8g2.print(czas_initial);
    u8g2.setCursor(45, 63);
    u8g2.print(czas_bulk);
    u8g2.setCursor(88, 63);
    u8g2.print(czas_abs);

    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(1, 53);
    if (battery_type == battery_none) u8g2.print("Battery not defined");
    else if (battery_type == battery_std) u8g2.print("Batt std");
    else if (battery_type == battery_gel) u8g2.print("Gel batt");
    else if (battery_type == battery_agm) u8g2.print("Agm batt");
    else if (battery_type == battery_caca) u8g2.print("CaCa batt");
    else if (battery_type == battery_li_2s) u8g2.print("Li-Ion_2s");
    else if (battery_type == battery_li_3s) u8g2.print("Li-Ion_3s");

    u8g2.setCursor(55, 53);
    String x = String(battery[battery_type].capacity) + String("Ah");
    if (!(battery_type == battery_none)) u8g2.print(x);

    u8g2.setCursor(94, 53);
    if (!(battery_type == battery_none) && (charging_type == fast_charging)) u8g2.print("Fast");
    else if (!(battery_type == battery_none) && (charging_type == normal_charging)) u8g2.print("Normal");
    else if (!(battery_type == battery_none) && (charging_type == recond_charging)) u8g2.print("Recond");
    else if (!(battery_type == battery_none) && (asym_mode == true)) {
      u8g2.setCursor(88, 53);
      u8g2.print("Asymetry");
    }
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    if (save_to_sd == true) u8g2.drawGlyph(50, 10, 97);
    save_to_sd = false;
    if (wifi_status == 1) u8g2.drawGlyph(60, 10, 247);
    u8g2.sendBuffer();
  }

  if (page == 1) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont15_tf);
    u8g2.setCursor(45, 10);
    u8g2.print("Setup");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(10, 20);
    if ((charging_end == true) && (!(battery_type == battery_none))) u8g2.print("START");
    if (charging_end == false) u8g2.print("STOP");
    if (DischarSet && !DischarStart) u8g2.print("Disch.START");
    if (DischarSet && DischarStart) u8g2.print("Disch.STOP");
    u8g2.setCursor(10, 30);
    u8g2.print("LOADING");
    u8g2.setCursor(10, 40);
    u8g2.print("UNLOAD");
    u8g2.setCursor(10, 50);
    u8g2.print("TOOLS");
    u8g2.setCursor(10, 60);
    u8g2.print("EXIT");
    u8g2.sendBuffer();

    int x = 1;  // Domyślna pozycja x
    int y = 0;  // Zmienna y, którą ustawimy poniżej

    switch (menuitem) {
      case 1:
        y = 19;
        break;
      case 2:
        y = 29;
        break;
      case 3:
        y = 39;
        break;
      case 4:
        y = 49;
        break;
      case 5:
        y = 59;
        break;
      default:
        return;  // Wyjście z funkcji, jeśli menuitem jest poza zakresem
    }
    u8g2.setCursor(x, y);
    u8g2.print(">");
    // if (menuitem == 1) {
    //   u8g2.setCursor(1, 19);
    //   u8g2.print(">");
    // }
    // if (menuitem == 2) {
    //   u8g2.setCursor(1, 29);
    //   u8g2.print(">");
    // }
    // if (menuitem == 3) {
    //   u8g2.setCursor(1, 39);
    //   u8g2.print(">");
    // }
    // if (menuitem == 4) {
    //   u8g2.setCursor(1, 49);
    //   u8g2.print(">");
    // }
    // if (menuitem == 5) {
    //   u8g2.setCursor(1, 59);
    //   u8g2.print(">");
    // }

    u8g2.sendBuffer();
  }

  if (page == 12) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont15_tf);
    u8g2.setCursor(25, 10);
    u8g2.print("Battery");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(10, 20);
    u8g2.print("Standard");
    u8g2.setCursor(10, 30);
    u8g2.print("Gel");
    u8g2.setCursor(10, 40);
    u8g2.print("AGM");
    u8g2.setCursor(10, 50);
    u8g2.print("CaCa");
    u8g2.setCursor(10, 60);
    u8g2.print("Li-Ion 2s");
    u8g2.setCursor(70, 50);
    u8g2.print("Li-Ion 3s");
    u8g2.setCursor(100, 60);
    u8g2.print("Exit");
    u8g2.sendBuffer();

    int x = 1;  // Domyślna pozycja x
    int y = 0;  // Zmienna y, którą ustawimy poniżej

    switch (menuitem) {
      case 1:
        y = 19;
        break;
      case 2:
        y = 29;
        break;
      case 3:
        y = 39;
        break;
      case 4:
        y = 49;
        break;
      case 5:
        y = 59;
        break;
      case 6:
        x = 60;  // Specyficzna pozycja x dla menuitem == 6
        y = 49;
        break;
      case 7:
        x = 91;  // Specyficzna pozycja x dla menuitem == 7
        y = 59;
        break;
      default:
        return;  // Wyjście z funkcji, jeśli menuitem jest poza zakresem 1-7
    }

    u8g2.setCursor(x, y);
    u8g2.print(">");
    u8g2.sendBuffer();
  }

  if (page == 123) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_luRS12_tf);
    u8g2.setCursor(25, 15);
    u8g2.print("Capacity");
    u8g2.setFont(u8g2_font_crox5tb_tf);
    u8g2.setCursor(25, 45);
    u8g2.print(battery[battery_type].capacity);
    u8g2.print("Ah");
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(1, 63);
    if (battery_type == battery_none) u8g2.print("Battery not defined");
    else if (battery_type == battery_std) u8g2.print("Battery Stdandard");
    else if (battery_type == battery_gel) u8g2.print("Battery Gel");
    else if (battery_type == battery_agm) u8g2.print("Battery Agm");
    else if (battery_type == battery_caca) u8g2.print("Battery CaCa");
    else if (battery_type == battery_li_2s) u8g2.print("Battery Li-Ion2s");
    else if (battery_type == battery_li_3s) u8g2.print("Battery Li-Ion3s");
    u8g2.sendBuffer();
  }
  if (page == 124) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont15_tf);
    u8g2.setCursor(20, 15);
    u8g2.print("Charging mode");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(15, 30);
    u8g2.print("Normal");
    u8g2.setCursor(15, 40);
    u8g2.print("Fast");
    if (!((battery_type == battery_li_2s) || (battery_type == battery_li_3s))) {
      u8g2.setCursor(15, 50);
      u8g2.print("Recond");
      u8g2.setCursor(15, 60);
      u8g2.print("Asymetry");
    }
    int x = 1;  // Domyślna pozycja x
    int y = 0;  // Zmienna y, którą ustawimy poniżej

    switch (menuitem) {
      case 1:
        y = 29;
        break;
      case 2:
        y = 39;
        break;
      case 3:
        y = 49;
        break;
      case 4:
        y = 59;
        break;
      default:
        return;  // Wyjście z funkcji, jeśli menuitem jest poza zakresem
    }
    u8g2.setCursor(x, y);
    u8g2.print(">");
    // if (menuitem == 1) {
    //   u8g2.setCursor(1, 29);
    //   u8g2.print(">");
    // }
    // if (menuitem == 2) {
    //   u8g2.setCursor(1, 39);
    //   u8g2.print(">");
    // }
    // if (menuitem == 3) {
    //   u8g2.setCursor(1, 49);
    //   u8g2.print(">");
    // }
    // if (menuitem == 4) {
    //   u8g2.setCursor(1, 59);
    //   u8g2.print(">");
    // }
    u8g2.sendBuffer();
  }

  if (page == 125) {

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_luRS12_tf);
    u8g2.setCursor(25, 15);
    u8g2.print("Li-ion type");
    u8g2.setFont(u8g2_font_crox5tb_tf);
    u8g2.setCursor(25, 45);
    u8g2.print(li_battery_type);
    u8g2.print("P");
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setCursor(1, 63);
    if (battery_type == battery_none) u8g2.print("Battery not defined");
    else if (battery_type == battery_std) u8g2.print("Battery Stdandard");
    else if (battery_type == battery_gel) u8g2.print("Battery Gel");
    else if (battery_type == battery_agm) u8g2.print("Battery Agm");
    else if (battery_type == battery_caca) u8g2.print("Battery CaCa");
    else if (battery_type == battery_li_2s) u8g2.print("Battery Li-Ion2s");
    else if (battery_type == battery_li_3s) u8g2.print("Battery Li-Ion3s");
    u8g2.sendBuffer();
  }
  if (page == 13) {
    float prevRshunt = 0.0;
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont15_tf);
    u8g2.setCursor(40, 11);
    u8g2.print("Rshunt");
    u8g2.setFont(u8g2_font_fur11_tf);
    u8g2.setCursor(25, 28);
    String x = String(Rshunt, 4) + " Ohm";
    //String x = " Ohm";
    u8g2.print(x);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(1, 37);
    String y = "CurrRaw " + String(cur_sampleRaw);
    u8g2.print(y);
    u8g2.setCursor(1, 50);
    //u8g2.print(str);
    u8g2.setCursor(90, 63);
    u8g2.print(buf2);
    u8g2.setCursor(1, 63);
    u8g2.print("cal=");
    u8g2.setCursor(23, 63);
    u8g2.print(ina219.cal_value());
    u8g2.sendBuffer();
    if (Rshunt != prevRshunt) ina219.calibrate(Rshunt, V_SHUNT_MAX, V_BUS_MAX, I_MAX_EXPECTED);
    prevRshunt = Rshunt;
  }

  if (page == 131) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_luRS12_tf);
    u8g2.setCursor(15, 15);
    u8g2.print("UNLOAD");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(15, 30);
    u8g2.print("Asymetry");
    u8g2.setCursor(15, 45);
    u8g2.print("CC Disch.");
    u8g2.setCursor(15, 60);
    u8g2.print("Exit");
    if (menuitem == 1) {
      u8g2.setCursor(1, 29);
      u8g2.print(">");
    }
    if (menuitem == 2) {
      u8g2.setCursor(1, 44);
      u8g2.print(">");
    }
    if (menuitem == 3) {
      u8g2.setCursor(1, 59);
      u8g2.print(">");
    }

    u8g2.sendBuffer();
  }

  if (page == 132) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_luRS12_tf);
    u8g2.setCursor(15, 15);
    u8g2.print("Asym.value");
    u8g2.setFont(u8g2_font_crox5tb_tf);
    u8g2.setCursor(45, 45);
    String x = "";
    if (asym_val == 10) {
      x += "10%";
      u8g2.print(x);
      //Serial.println("10%");
    }
    if (asym_val == 20) {
      x += "20%";
      u8g2.print(x);
      //Serial.println("20%");
    }
    if (asym_val == 30) {
      x += "30%";
      u8g2.print(x);
      //Serial.println("30%");
    }
    u8g2.sendBuffer();
  }


  if (page == 133) {
    String x = String(CurrentDisch) + "A";
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont15_tf);
    u8g2.setCursor(1, 20);
    u8g2.print("Set Current Disch.");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(10, 30);
    u8g2.print("Curr = ");
    u8g2.println(x);
    u8g2.sendBuffer();
  }
  if (page == 14) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont15_tf);
    u8g2.setCursor(45, 12);
    u8g2.print("TOOLS");  // page 141

    u8g2.setFont(u8g2_font_ncenB08_tr);
    for (int i = 0; i < 4; i++) {  //ilość wierszy jednocześnie
      if (topItem + i < totalItemsP14) {
        if (topItem + i == menuitem - 1) {
          // Draw selection background
          u8g2.setDrawColor(1);  // Set draw color to white
          u8g2.drawBox(0, 20 + i * 10, 128, 10);

          // Draw text in inverted color
          u8g2.setDrawColor(0);  // Set draw color to black
          u8g2.setCursor(10, 29 + i * 10);
          u8g2.print(menuItemsP14[topItem + i]);

          // Reset draw color to white for next items
          u8g2.setDrawColor(1);
        } else {
          // Draw normal text
          u8g2.setDrawColor(1);  // Set draw color to white
          u8g2.setCursor(10, 29 + i * 10);
          u8g2.print(menuItemsP14[topItem + i]);
        }
      }
    }




    // for (int i = 0; i < visibleItems; i++) {
    //   int menuItemIndex = topItem + i;
    //   if (menuItemIndex < totalItems) {
    //     int yPosition = (i + 1) * 12 + 10;  // Pozycja y dla 1 opcji menu, zaczynając od drugiej linijki dla 0 y = 22
    //     if (menuItemIndex == menuitem) {
    //       u8g2.setDrawColor(1);                      // Tekst wyróżniony
    //       u8g2.drawBox(0, yPosition, 128, 12);  // Wyróżnienie tła
    //       u8g2.setDrawColor(0);                      // Tekst czarny na wyróżnionym tle
    //     } else {
    //       u8g2.setDrawColor(1);  // Normalny tekst
    //     }
    //     u8g2.setCursor(1, yPosition);
    //     u8g2.print(menuItems[menuItemIndex]);
    //   }
    // }


    u8g2.sendBuffer();
  }
  if (page == 141) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(30, 15);
    u8g2.print("TOOLS/FAN");
    u8g2.setFont(u8g2_font_crox5tb_tf);
    u8g2.setCursor(28, 45);
    if (fan_auto == true) u8g2.print("AUTO");
    if (fan_auto == false) {
      u8g2.setCursor(8, 45);
      u8g2.print("MANUAL");
    }
    u8g2.sendBuffer();
  }
  if (page == 1411) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_11b_tf);
    u8g2.setCursor(16, 15);
    u8g2.print("TOOLS/FAN/MANUAL");
    u8g2.setFont(u8g2_font_crox5tb_tf);
    u8g2.setCursor(40, 45);
    String x = String(fan_manual) + "%";
    u8g2.print(x);
    u8g2.sendBuffer();
  }
  if (page == 142) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(25, 15);
    u8g2.print("TOOLS/PWM");
    u8g2.setFont(u8g2_font_crox5tb_tf);
    u8g2.setCursor(25, 45);
    if (pwm_auto == true) u8g2.print("AUTO");
    if (pwm_auto == false) {
      u8g2.setCursor(8, 45);
      u8g2.print("MANUAL");
    }
    u8g2.sendBuffer();
  }
  if (page == 1421) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_11b_tf);
    u8g2.setCursor(16, 15);
    u8g2.print("TOOLS/PWM/MANUAL");
    u8g2.setFont(u8g2_font_crox5tb_tf);
    u8g2.setCursor(50, 45);
    u8g2.print(pwm);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(10, 62);
    u8g2.print(buf1);
    u8g2.setCursor(55, 62);
    u8g2.print(buf2);
    u8g2.sendBuffer();
  }
  if (page == 143) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(30, 10);
    u8g2.print("TOOLS/SD");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(10, 30);
    u8g2.print("Interval");
    u8g2.setCursor(10, 50);
    u8g2.print("Exit");
    u8g2.setCursor(10, 40);
    if (auto_save_sd == true) u8g2.print("Auto save sd");
    if (auto_save_sd == false) u8g2.print("Manual save sd");

    int x = 1;  // Domyślna pozycja x
    int y = 0;  // Zmienna y, którą ustawimy poniżej

    switch (menuitem) {
      case 1:
        y = 29;
        break;
      case 2:
        y = 39;
        break;
      case 3:
        y = 49;
        break;
      default:
        return;  // Wyjście z funkcji, jeśli menuitem jest poza zakresem
    }
    u8g2.setCursor(x, y);
    u8g2.print(">");
    // if (menuitem == 1) {
    //   u8g2.setCursor(1, 29);
    //   u8g2.print(">");
    // }
    // if (menuitem == 2) {
    //   u8g2.setCursor(1, 39);
    //   u8g2.print(">");
    // }
    // if (menuitem == 3) {
    //   u8g2.setCursor(1, 49);
    //   u8g2.print(">");
    // }
    u8g2.sendBuffer();
  }
  if (page == 1431) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_11b_tf);
    u8g2.setCursor(10, 15);
    u8g2.print("TOOLS/SD/INTERVAL");
    u8g2.setFont(u8g2_font_crox5tb_tf);
    u8g2.setCursor(35, 45);
    String x = String(sd_interval / 1000) + "sek";
    u8g2.print(x);
    u8g2.sendBuffer();
  }
  if (page == 144) {
    if (battery_type == battery_none) {
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.setCursor(1, 8);
      u8g2.print("Battery type undefined");
      u8g2.sendBuffer();
    } else {
      if (battery_type == battery_std) u8g2.print("Standard");
      else if (battery_type == battery_gel) u8g2.print("Gel");
      else if (battery_type == battery_agm) u8g2.print("Agm");
      else if (battery_type == battery_caca) u8g2.print("CaCa");
      else if (battery_type == battery_li_2s) u8g2.print("Li-Ion2s");
      else if (battery_type == battery_li_3s) u8g2.print("Li-Ion3s");
      u8g2.clearBuffer();
      u8g2.setCursor(50, 8);
      u8g2.print("batt settings");
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.setCursor(10, 20);
      u8g2.print("u_bulk");
      u8g2.setCursor(10, 30);
      u8g2.print("u_float");
      u8g2.setCursor(10, 40);
      u8g2.print("u_recond");
      u8g2.setCursor(10, 50);
      u8g2.print("u_storage");
      u8g2.setCursor(10, 60);
      u8g2.print("---");
      u8g2.setCursor(100, 60);
      u8g2.print("Exit");

      u8g2.setCursor(65, 20);
      u8g2.print(battery[battery_type].u_bulk);
      u8g2.setCursor(65, 30);
      u8g2.print(battery[battery_type].u_float);
      u8g2.setCursor(65, 40);
      u8g2.print(battery[battery_type].u_recond);
      u8g2.setCursor(65, 50);
      u8g2.print(battery[battery_type].u_storage);

      if (menuitem == 1) {
        u8g2.setCursor(1, 19);
        u8g2.print(">");
      }
      if (menuitem == 2) {
        u8g2.setCursor(1, 29);
        u8g2.print(">");
      }
      if (menuitem == 3) {
        u8g2.setCursor(1, 39);
        u8g2.print(">");
      }
      if (menuitem == 4) {
        u8g2.setCursor(1, 49);
        u8g2.print(">");
      }
      if (menuitem == 5) {
        u8g2.setCursor(90, 59);
        u8g2.print(">");
      }
      u8g2.sendBuffer();
    }
  }
  if (page == 146) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_11b_tf);
    u8g2.setCursor(30, 15);
    u8g2.print("TOOLS/PID/");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(10, 30);
    u8g2.print("Kp = ");
    u8g2.println(kp);
    u8g2.setCursor(10, 40);
    u8g2.print("Ki = ");
    u8g2.println(ki);
    u8g2.setCursor(10, 50);
    u8g2.print("Kd = ");
    u8g2.println(kd);
    u8g2.setCursor(10, 60);
    u8g2.print("Exit");
    u8g2.setCursor(50, 60);
    u8g2.print(buf1);  // napięcie
    u8g2.setCursor(100, 60);
    u8g2.print(pidValue);


    if (menuitem == 1) {
      u8g2.setCursor(1, 29);
      u8g2.print(">");
    }
    if (menuitem == 2) {
      u8g2.setCursor(1, 39);
      u8g2.print(">");
    }
    if (menuitem == 3) {
      u8g2.setCursor(1, 49);
      u8g2.print(">");
    }
    if (menuitem == 4) {
      u8g2.setCursor(1, 59);
      u8g2.print(">");
    }



    u8g2.sendBuffer();
  }
  if (page == 1441) {
    // u8g2.clearBuffer();
    // u8g2.setFont(u8g2_font_t0_11b_tf);
    // u8g2.setCursor(16, 15);
    // u8g2.print("TOOLS/PRAD/MANUAL");
    // u8g2.setFont(u8g2_font_crox5tb_tf);
    // u8g2.setCursor(30, 45);
    // String x = String(i_manual) + "A";
    // u8g2.print(x);
    // u8g2.sendBuffer();
  }
}
