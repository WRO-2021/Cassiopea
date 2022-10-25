#ifndef motors_h
#define motors_h

#include <Arduino.h>

#define MOTOR_A1 8
#define MOTOR_A2 7
#define MOTOR_APWM 6

#define MOTOR_B1 9
#define MOTOR_B2 10
#define MOTOR_BPWM 11

#define MOTOR_A_MULTIPLIER 1
#define MOTOR_B_MULTIPLIER 0.98


void motor_init();

void motor_set_speed_A(uint8_t vel);
void motor_set_speed_B(uint8_t vel);
void motor_set_speed_both(uint8_t vel);

void motor_left();
void motor_right();
void motor_backward();
void motor_straight();
void motor_stop();
void motor_break();

#endif
