
#include <Preferences.h>

//-----------------------------------------------------------------------------------------------------------------------//

#include "display.h"
#include "check_page.h"
#include "common.h"
#include "sdcard.h"

//-----------------------------------------------------------------------------------------------------------------------//

extern TBATTERY_TYPE battery_type;
extern TBATT battery[7];
bool XloadingInfo = false;

void check_page() {

  if (up && page == 0) {
    if (pwm_auto == false) {
      up = false;
      pwm -= 1;
      charge_pwm_duty();
    }
  }

  if (up && page == 1) {
    up = false;
    menuitem++;
    if (battery_type == battery_none) {
      if (menuitem == 6) menuitem = 2;
    } else if ((!battery_type) == battery_none) {
      if (menuitem == 6) menuitem = 1;
    }
  } else if (up && page == 12) {
    up = false;
    menuitem++;
    if (menuitem == 8) {
      menuitem = 1;
    }
  } else if (up && page == 121) {
    up = false;
    umin -= 0.1;
  } else if (up && page == 122) {
    up = false;
    if (battery_type == battery_std) battery[0].u_bulk -= 0.1;
    if (battery_type == battery_gel) battery[1].u_bulk -= 0.1;
    if (battery_type == battery_agm) battery[2].u_bulk -= 0.1;
    if (battery_type == battery_caca) battery[3].u_bulk -= 0.1;
    if (battery_type == battery_li_2s) battery[4].u_bulk -= 0.1;
    if (battery_type == battery_li_3s) battery[5].u_bulk -= 0.1;
  } else if (up && page == 123) {
    up = false;
    battery[battery_type].capacity += 1;

    if (battery[battery_type].capacity <= test_capacity) {
      bigbattery = false;
    } else {
      bigbattery = true;
    }
  } else if (up && page == 124) {
    up = false;
    menuitem++;
    if (menuitem == 5) {
      menuitem = 1;
    }

    //capacity -= 1;
  } else if (up && page == 125) {
    up = false;
    li_battery_type += 1;
  } else if (up && page == 13) {
    up = false;
    Rshunt -= 0.0001;
  } else if (up && page == 131) {
    up = false;
    menuitem++;
    if (menuitem > 3) {
      menuitem = 1;
    }
  } else if (up && page == 132) {
    up = false;
    asym_val += 10;
    if (asym_val > 30) asym_val = 30;
  }

  else if (up && page == 133) {
    up = false;
    CurrentDisch += 0.1;
    if (CurrentDisch > 4.0) {
      CurrentDisch = 0.1;
    }
    if (CurrentDisch > 0.0) DischarSet = true;
    if (CurrentDisch == 0.0) DischarSet = false;
  }

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  else if (up && page == 14) {
    up = false;
    if (menuitem < totalItemsP14) {
      menuitem++;
      if (menuitem >= topItem + visibleItems) {
        topItem++;
      }
    }
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  else if (up && page == 141) {
    up = false;
    fan_auto = false;
  } else if (up && page == 1411) {
    up = false;
    fan_manual -= 1;
  } else if (up && page == 142) {
    up = false;
    pwm_auto = false;
    pwm_manual = 0;
  } else if (up && page == 1421) {
    up = false;
    pwm += 1;
    charge_pwm_duty();
  } else if (up && page == 143) {
    up = false;
    menuitem++;
    if (menuitem == 4) {
      menuitem = 1;
    }
  } else if (up && page == 1431) {
    up = false;
    sd_interval += 1000;
  }

  else if ((up && page == 144) && (menuitem_stop == false)) {
    up = false;
    menuitem++;
    if (menuitem == 6) {
      menuitem = 1;
    }
  } else if ((up && page == 144) && (menuitem_stop == true)) {
    up = false;
    if (menuitem == 1) battery[battery_type].u_bulk += 0.1;
    if (menuitem == 2) battery[battery_type].u_float += 0.1;
    if (menuitem == 3) battery[battery_type].u_recond += 0.1;
    if (menuitem == 4) battery[battery_type].u_storage += 0.1;
  } else if (up && page == 1441) {
    up = false;
    i_manual -= 0.1;
  }

  else if ((up && page == 146) && (menuitem_stop == false)) {
    up = false;
    menuitem++;
    if (menuitem == 5) {
      menuitem = 1;
    }
  } else if ((up && page == 146) && (menuitem_stop == true)) {
    up = false;
    if (menuitem == 1) kp += 100;
    if (menuitem == 2) ki += 1.0;
    if (menuitem == 3) kd += 1.0;
  }
  //++++++++++++++++++++++++++++++++++++++++++++ DOWN ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


  if (down && page == 0) {
    if (pwm_auto == false) {
      down = false;
      pwm += 1;
      charge_pwm_duty();
    }
  }

  if (down && page == 1) {
    down = false;
    menuitem--;
    if (battery_type == battery_none) {
      if (menuitem == 1) {
        menuitem = 5;
      }
    } else {
      if (menuitem == 0) {
        menuitem = 5;
      }
    }

  } else if (down && page == 12) {
    down = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 7;
    }
  } else if (down && page == 121) {
    down = false;
    //clear_disp();
    umin += 0.1;
  } else if (down && page == 122) {
    down = false;
    if (battery_type == battery_std) battery[0].u_bulk += 0.1;
    if (battery_type == battery_gel) battery[1].u_bulk += 0.1;
    if (battery_type == battery_agm) battery[2].u_bulk += 0.1;
    if (battery_type == battery_caca) battery[3].u_bulk += 0.1;
    if (battery_type == battery_li_2s) battery[4].u_bulk += 0.1;
    if (battery_type == battery_li_3s) battery[5].u_bulk += 0.1;
  } else if (down && page == 123) {
    down = false;
    battery[battery_type].capacity -= 1;
    if (battery[battery_type].capacity <= test_capacity) {
      bigbattery = false;
    } else {
      bigbattery = true;
    }
  } else if (down && page == 124) {
    down = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 4;
    }
  } else if (down && page == 125) {
    down = false;
    li_battery_type -= 1;
  } else if (down && page == 13) {
    down = false;
    Rshunt += 0.0001;
  } else if (down && page == 131) {
    down = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 3;
    }
  }

  else if (down && page == 132) {
    down = false;
    asym_val -= 10;
    if (asym_val <= 10) asym_val = 10;
  }

  else if (down && page == 133) {
    down = false;
    CurrentDisch -= 0.10;
    if (CurrentDisch < 0.0) {
      CurrentDisch = 4.0;
    }
    if (CurrentDisch > 0.0) DischarSet = true;
    if (CurrentDisch == 0.0) DischarSet = false;
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  else if (down && page == 14) {
    down = false;
    if (menuitem > 1) {
      menuitem--;
      if (menuitem > topItem) {
        topItem--;
        if (topItem > 254) topItem = 0;
      }
    }
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  else if (down && page == 141) {
    down = false;
    fan_auto = true;
  } else if (down && page == 1411) {
    down = false;
    fan_manual += 1;
  } else if (down && page == 142) {
    down = false;
    pwm_auto = true;
  } else if (down && page == 1421) {
    down = false;
    pwm -= 1;
    charge_pwm_duty();
  } else if (down && page == 143) {
    down = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 3;
    }
  } else if (down && page == 1431) {
    down = false;
    sd_interval -= 1000;
  }

  else if ((down && page == 144) && (menuitem_stop == false)) {
    down = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 5;
    }
  } else if ((down && page == 144) && (menuitem_stop == true)) {
    down = false;
    if (menuitem == 1) battery[battery_type].u_bulk -= 0.1;
    if (menuitem == 2) battery[battery_type].u_float -= 0.1;
    if (menuitem == 3) battery[battery_type].u_recond -= 0.1;
    if (menuitem == 4) battery[battery_type].u_storage -= 0.1;
  }

  else if (down && page == 1441) {
    down = false;
    i_manual += 0.1;
  }

  else if ((down && page == 146) && (menuitem_stop == false)) {
    down = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 4;
    }
  } else if ((down && page == 146) && (menuitem_stop == true)) {
    down = false;
    if (menuitem == 1) kp -= 100;
    if (menuitem == 2) ki -= 1;
    if (menuitem == 3) kd -= 1;
  } else if (up && page == 1441) {
    up = false;
    i_manual -= 0.1;
  }
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  if (middle) {
    middle = false;

    if (page == 0) {
      page = 1;
      menuitem = 2;
    }


    else if (page == 1 && menuitem == 1) {

      String Sbattery_type = "";
      String Scharging_type = "";


      if (battery_type == battery_std) Sbattery_type = "Battery_STD ";
      else if (battery_type == battery_gel) Sbattery_type = "Battery_GEL ";
      else if (battery_type == battery_agm) Sbattery_type = "Battery_AGM ";
      else if (battery_type == battery_caca) Sbattery_type = "Battery_CaCa ";
      else if (battery_type == battery_li_2s) Sbattery_type = "Battery_Li2s ";
      else if (battery_type == battery_li_3s) Sbattery_type = "Battery_Li3s ";

      if (charging_type == normal_charging) Scharging_type = "Normal charging";
      else if (charging_type == fast_charging) Scharging_type = "Fast charging";
      else if (charging_type == recond_charging) Scharging_type = "Recond charging";
      else if (charging_type == asymetry_charging) Scharging_type = "Asymetry charging";

      String loadingInfo = Sbattery_type + String(uint8_t(battery[battery_type].capacity)) + "Ah" + " " + Scharging_type + "\r\n";  // start/stop
      if (!XloadingInfo) {
        appendFile(SD, stats_filename, loadingInfo.c_str());  // zapisz dane tylko jeden raz
        appendFile(SD, stats_filename, "\r\n");
        appendFile(SD, stats_filename, "----------------------------------- \r\n");
      }
      XloadingInfo = true;
      diff_save_sd = true;
      logger_time();



      if (DischarSet == false) charging_end = !(charging_end);
      else if (DischarSet == true) DischarStart = !DischarStart;
      sd_seconds = 0;
      sd_minutes = 0;
      if ((charging_end) && (!DischarStart)) {
        stats_log();
      }


      //if(!DischarStart) stats_log();
      charger_state = off;

      page = 0;

    }







    else if (page == 1 && menuitem == 2) {
      ChargingMode = true;
      page = 12;
      menuitem = 1;
    } else if (page == 1 && menuitem == 3) {
      DischargingMode = true;
      page = 123;
      menuitem = 1;
    } else if (page == 1 && menuitem == 4) {
      //clear_disp();
      page = 14;
      menuitem = 1;
    } else if (page == 1 && menuitem == 5) {
      //clear_disp();
      page = 0;
      menuitem = 1;
    }

    else if (page == 1 && menuitem == 1 && charging_end == true && !battery_type == battery_none && busvoltage >= MIN_BAT_VOLTS) {
      charging_end = false;
      //sekundy = 0;
      //minuty = 0;//-----------reset czasu
      //godziny = 0;
      page = 0;
      //clear_disp();
    } else if (page == 12 && menuitem == 1) {
      battery_type = battery_std;
      storage_batt_volts = battery[1].u_storage;
      ufloat = battery[1].u_float;
      u_bulk = battery[1].u_bulk;
      //set_bat();
      page = 123;
      menuitem = 1;
    } else if (page == 12 && menuitem == 2) {
      battery_type = battery_gel;
      storage_batt_volts = battery[2].u_storage;
      ufloat = battery[2].u_float;
      u_bulk = battery[2].u_bulk;
      page = 123;
      menuitem = 1;
    } else if (page == 12 && menuitem == 3) {
      battery_type = battery_agm;
      storage_batt_volts = battery[3].u_storage;
      ufloat = battery[3].u_float;
      u_bulk = battery[3].u_bulk;
      page = 123;
      menuitem = 1;
    } else if (page == 12 && menuitem == 4) {
      battery_type = battery_caca;
      storage_batt_volts = battery[4].u_storage;
      if (charging_type == recond_charging) u_bulk = 16.2;
      else if (!(charging_type == recond_charging)) u_bulk = battery[4].u_bulk;
      ufloat = battery[4].u_float;
      page = 123;
      menuitem = 1;
    } else if (page == 12 && menuitem == 5) {
      battery_type = battery_li_2s;
      storage_batt_volts = battery[5].u_storage;
      ufloat = battery[5].u_float;
      u_bulk = battery[5].u_bulk;
      page = 123;
      menuitem = 1;
    } else if (page == 12 && menuitem == 6) {
      battery_type = battery_li_3s;
      storage_batt_volts = battery[6].u_storage;
      ufloat = battery[6].u_float;
      u_bulk = battery[6].u_bulk;
      page = 123;
      menuitem = 1;
    } else if (page == 12 && menuitem == 7) {
      page = 1;
      menuitem = 1;
    } else if (page == 121) {
      //clear_disp();
      page = 12;
    } else if (page == 122) {
      //clear_disp();
      page = 12;
    } else if (page == 123) {
      if (ChargingMode) {
        if (bigbattery) {
          intervals_size = 18;
          for (uint8_t i = 0; i < intervals_size; i++) {
            intervals[i] = intervals_h[i];
          }
        } else {
          intervals_size = 26;
          for (uint8_t i = 0; i < intervals_size; i++) {
            intervals[i] = intervals_min[i];
          }
        }
        Serial.print("intervals_size=");
        Serial.println(intervals_size);
        li_normal_mode = 2;  //battery[battery_type].capacity / 2;
        li_fast_mode = 2.5;  //battery[battery_type].capacity;
        for (int i = 0; i < intervals_size; i++) {
          Serial.println(intervals[i]);
        }
        if ((battery_type == battery_li_2s) || (battery_type == battery_li_3s)) page = 125;
        else page = 124;
      } else {
        page = 133;
      }
    }



    else if (page == 124) {
      if (menuitem == 1) {
        //charging_mode = false;
        charging_type = normal_charging;
        asym_mode = false;
      }
      if (menuitem == 2) {
        //charging_mode = true;
        charging_type = fast_charging;
      }
      if (menuitem == 3) {
        //charging_mode = false;
        charging_type = recond_charging;
        //recond = true;
      }
      if (menuitem == 4) {
        charging_type = asymetry_charging;
        asym_mode = true;
        //recond = true;
      }
      page = 1;
      menuitem = 1;
    } else if (page == 125) {
      page = 124;
    } else if (page == 13) {
      preferences.putFloat("rshunt", Rshunt);
      page = 1;
    } else if (page == 131 && menuitem == 1) {
      page = 132;
      menuitem = 1;
    } else if (page == 131 && menuitem == 2) {
      page = 123;
      menuitem = 1;
    } else if (page == 131 && menuitem == 3) {
      page = 1;
      menuitem = 1;
    } else if (page == 132) {
      page = 131;
      menuitem = 1;
    } else if (page == 133) {
      page = 1;
      menuitem = 1;
    }

    else if (page == 1331) {
      page = 131;
      menuitem = 3;
    }

    else if (page == 14 && menuitem == 1) {
      page = 141;
      menuitem = 1;
    } else if (page == 14 && menuitem == 2) {
      page = 142;
      menuitem = 1;
    } else if (page == 14 && menuitem == 3) {
      page = 143;
      menuitem = 1;
    } else if (page == 14 && menuitem == 4) {
      page = 144;
      menuitem = 1;
      topItem = 0;
    } else if (page == 14 && menuitem == 5) {
      //clear_disp();
      page = 13;
      topItem = 0;
    } else if (page == 14 && menuitem == 6) {
      menuitem = 1;
      page = 146;
      topItem = 0;
    } else if (page == 14 && menuitem == 7) {
      menuitem = 4;
      page = 1;
      topItem = 0;
    } else if ((page == 141) && (fan_auto == false)) {
      //clear_disp();
      page = 1411;
    } else if ((page == 141) && (fan_auto == true)) {
      //clear_disp();
      page = 14;
    } else if (page == 1411) {
      //clear_disp();
      page = 14;
    } else if ((page == 142) && (pwm_auto == false)) {
      //clear_disp();
      page = 1421;
      pwm = 0;
      charge_pwm_duty();
    } else if ((page == 142) && (pwm_auto == true)) {
      //clear_disp();
      page = 14;
      menuitem = 2;
    } else if (page == 1421) {
      page = 14;
      menuitem = 2;
    } else if (page == 143 && menuitem == 1) {
      page = 1431;
    } else if (page == 143 && menuitem == 2) {
      auto_save_sd = !auto_save_sd;
    } else if (page == 143 && menuitem == 3) {
      page = 14;
    } else if (page == 1431) {
      //clear_disp();
      page = 143;
      menuitem = 2;
    }

    //****************************************************************************************

    else if ((page == 144) && (battery_type == battery_none)) {
      page = 14;
      menuitem = 4;
    } else if ((page == 144) && !(menuitem == 5) && (menuitem_stop == false)) {
      menuitem_stop = !menuitem_stop;
    } else if ((page == 144) && !(menuitem == 5) && (menuitem_stop == true)) {
      menuitem_stop = !menuitem_stop;
    } else if ((page == 144) && (menuitem == 5)) {
      menuitem_stop = false;
      page = 14;
      menuitem = 4;
    }
    //****************************************************************************************

    else if ((page == 146) && (menuitem == 4)) {
      page = 14;
      menuitem = 1;
    } else if ((page == 146) && !(menuitem == 4) && (menuitem_stop == false)) {
      menuitem_stop = !menuitem_stop;
    } else if ((page == 146) && !(menuitem == 4) && (menuitem_stop == true)) {
      menuitem_stop = !menuitem_stop;
    }
    // else if ((page == 144) && (menuitem == 4)) {
    //   menuitem_stop = false;
    //   page = 14;
    //   menuitem = 6;
    // }
    //****************************************************************************************
    else if (page == 1441) {
      //clear_disp();
      page = 14;
      menuitem = 4;
    }
  }
}
