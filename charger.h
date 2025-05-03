
#ifndef CHARGER_H
#define CHARGER_H


extern void run_charger(void);
extern void charge_pwm_duty();
extern void pid_pwm_duty();
extern void discharge_pwm_duty(uint16_t);
extern void alarm_led(uint16_t x, byte z);

void handle_asym_mode();  
void handle_bulk_lipo();
void handle_bulk_standard();
void log_absorption_currents();
bool check_absorption_end_conditions();
bool check_absorption_li_ion_end();

extern ChargerTime chargerTime[6];
extern TBATT battery[7];
extern TCHARGER_STATE charger_state;
extern TBATTERY_TYPE battery_type;

extern float dischVoltage;

extern uint16_t intervals_min[];
extern uint16_t intervals_h[];
extern uint16_t intervals[];
extern bool AutomaticEndOfCharging;
//extern uint8_t intervals_size;  extern nie jest potrzebne

#endif
