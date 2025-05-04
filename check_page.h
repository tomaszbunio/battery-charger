
#ifndef CHECK_PAGE_H
#define CHECK_PAGE_H


extern void check_page(void);
extern void charge_pwm_duty();
extern void stats_log(void);
extern void set_bat();
extern void bat_init();
extern void reset_charger_state();

extern uint16_t intervals_min[26];
extern uint16_t intervals_h[18];
extern uint16_t intervals[];

extern bool DischarStart;
extern bool DischarSet;
extern bool ChargingMode;
extern bool DischargingMode;

extern uint8_t abs_min_intervals;
extern uint8_t abs_h_intervals;
extern uint16_t intervals_size;

extern class Preferences preferences;
extern class AiEsp32RotaryEncoder rotaryEncoder;

extern const uint8_t fastt;
extern const uint8_t normall;

extern bool up;
extern bool down;
extern bool middle;
extern bool menuitem_stop;

extern uint16_t page;
extern uint8_t menuitem;

extern float li_normal_mode;
extern float li_fast_mode;
//extern  uint8_t recond_charging;

extern float u_min;
extern float u_bulk;
extern float u_float;
extern float u_recond;
extern float storage_batt_volts;

extern float i_fast;
extern float i_std;
extern float i_recond;
extern float i_manual;
extern float test_capacity;

extern float Rshunt;
extern float pojemnosc;
extern bool fan_auto;
extern uint8_t fan_manual;
extern uint16_t pwm_manual;
extern bool pwm_auto;
extern uint16_t sd_interval;
extern bool current_automode;

extern bool charging_end;
extern bool auto_save_sd;

extern byte mode_C20;
extern byte mode_C10;
extern byte mode_C8;







#endif
