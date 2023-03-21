#ifndef CASSIOPEA_TOF_H
#define CASSIOPEA_TOF_H

#include <Arduino.h>
#include <Wire.h>
#include <VL6180X.h>

void MUX(uint8_t bus);

int tof_get_offset(uint8_t sens);

int tof_read(uint8_t sens);

void tof_conf(uint8_t sens);

class tof{
public:
    tof(uint8_t bus);

    int read();
    int read_with_offset();
    void conf();

    static tof get_instance(uint8_t index);
    static tof get_in_direction(int direction, int num=0);

private:
    uint8_t bus;
    VL6180X sensor;

    void mux();

    static int tof_offset[];
    static vector<tof> instances;
};

#endif // CASSIOPEA_TOF_H
