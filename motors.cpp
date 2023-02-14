#include "motors.h"

void motor_init() {
    pinMode(MOTOR_A1, OUTPUT);
    pinMode(MOTOR_A2, OUTPUT);
    pinMode(MOTOR_APWM, OUTPUT);
    pinMode(MOTOR_B1, OUTPUT);
    pinMode(MOTOR_B2, OUTPUT);
    pinMode(MOTOR_BPWM, OUTPUT);

    motor_stop();
    motor_set_speed_both(127);
}


void motor_set_speed_both(uint8_t vel) {
    analogWrite(MOTOR_APWM, MOTOR_A_MULTIPLIER * vel);
    analogWrite(MOTOR_BPWM, MOTOR_B_MULTIPLIER * vel);
}

void motor_set_speed_A(uint8_t vel) {
    analogWrite(MOTOR_APWM, MOTOR_A_MULTIPLIER * vel);
}

void motor_set_speed_B(uint8_t vel) {
    analogWrite(MOTOR_BPWM, MOTOR_B_MULTIPLIER * vel);
}


void motor_straight() {
    digitalWrite(MOTOR_A1, LOW);
    digitalWrite(MOTOR_A2, HIGH);
    digitalWrite(MOTOR_B1, LOW);
    digitalWrite(MOTOR_B2, HIGH);
}

void motor_right() {
    digitalWrite(MOTOR_A1, HIGH);
    digitalWrite(MOTOR_A2, LOW);
    digitalWrite(MOTOR_B1, LOW);
    digitalWrite(MOTOR_B2, HIGH);
}

void motor_left() {

    digitalWrite(MOTOR_A1, LOW);
    digitalWrite(MOTOR_A2, HIGH);
    digitalWrite(MOTOR_B1, HIGH);
    digitalWrite(MOTOR_B2, LOW);
}

void motor_backward() {
    digitalWrite(MOTOR_A1, HIGH);
    digitalWrite(MOTOR_A2, LOW);
    digitalWrite(MOTOR_B1, HIGH);
    digitalWrite(MOTOR_B2, LOW);
}

void motor_stop() {
    digitalWrite(MOTOR_A1, LOW);
    digitalWrite(MOTOR_A2, LOW);
    digitalWrite(MOTOR_B1, LOW);
    digitalWrite(MOTOR_B2, LOW);
}

void motor_break() {
    digitalWrite(MOTOR_A1, HIGH);
    digitalWrite(MOTOR_A2, HIGH);
    digitalWrite(MOTOR_B1, HIGH);
    digitalWrite(MOTOR_B2, HIGH);
}
