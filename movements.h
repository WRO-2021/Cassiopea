#ifndef movements_h
#define movements_h

#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include "motors.h"
#include "tof.h"

float getOldAngleX();

void move_cm(int cm);

bool move_cm_avoid_black(int cm);

void gyro_calibrate(int num);

void turn_degrees(int deg);

void distanzia_muro();

void allinea_muro();

void allinea_muro_bene();

#endif
