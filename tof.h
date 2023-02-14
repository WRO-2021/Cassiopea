#ifndef tof_h
#define tof_h

#include <Arduino.h>
#include <Wire.h>
#include <VL6180X.h>

void MUX(uint8_t bus);

int tof_get_offset(uint8_t sens);

int tof_read(uint8_t sens);

void tof_conf(uint8_t sens);

void tof_conf_all();

#endif
