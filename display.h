
#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>


extern class U8G2_ST7567_ENH_DG128064I_F_HW_I2C u8g2;
extern class INA219 ina219;

extern void drawMenu(void);

 
extern uint8_t li_battery_type;
extern String czas;
extern uint16_t sd_num;
extern int pwm;
extern String czas_initial;
extern String czas_bulk;                         
extern String czas_abs;
extern float pojemnosc;
extern bool save_to_sd;
extern byte wifi_status;
extern bool charging_end;
extern float umin;
extern float u_bulk;
extern float ufloat;
extern float imax;
extern float Rshunt;
extern bool fan_auto;
extern uint8_t fan_manual;
extern uint16_t pwm_manual;
extern bool auto_save_sd;
extern uint16_t sd_interval;
extern bool i_auto;
//extern bool charging_mode;
extern float i_manual;
extern float test_capacity;

extern char buf1[10];     //busvoltage
extern char buf2[10];     //current_auto
extern char buf3[10];     //ampHours
extern char buf4[10];     //power_W
extern char buf5[10];     //wattHours
extern char buf6[10];     //localTime



extern uint16_t page;
//extern uint8_t menuitem;
extern uint8_t topItem;







#endif
