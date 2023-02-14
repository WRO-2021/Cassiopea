/**
 * source: https://github.com/pololu/vl6180x-arduino/blob/master/examples/InterleavedContinuous/InterleavedContinuous.ino
 */

#include "tof.h"


static VL6180X sensor[7];

static int tof_offset[] = {
        0,//bbboh
        -93,
        -97,
        -65,
        -63,
        -94,
        -102,
        -75
};

void MUX(uint8_t bus) {
    //set the transmission with the MUX
    Wire.beginTransmission(0x70);
    Wire.write(1 << bus);
    Wire.endTransmission();
}


int tof_read(uint8_t sens) {
    MUX(sens);
    int ret = sensor[sens].readRangeContinuousMillimeters();
    if (sensor[sens].timeoutOccurred()) {
        // sensore fa timeout, prova a riconfigurarlo, non e' una vera soluzione
        tof_conf(sens);
        ret = sensor[sens].readRangeContinuousMillimeters();
        if (sensor[sens].timeoutOccurred())
            return -1;
    }

    return ret;
}

void tof_conf_all() {
    for (int i = 0; i < 8; i++) {
        MUX(i);

        //read the value of the sensor every 100ms
        sensor[i].init();
        sensor[i].configureDefault();
        // per leggere una singola volta, piu' lentamente, si possono togliere queste due righe dopo
        sensor[i].writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
        sensor[i].writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 50);
        sensor[i].setTimeout(500);
        sensor[i].stopContinuous();
    }

    delay(300);

    for (int i = 0; i < 8; i++) {
        MUX(i);
        sensor[i].startInterleavedContinuous(100);
    }
}

void tof_conf(uint8_t sens) {
    MUX(sens);

    //read the value of the sensor every 100ms
    sensor[sens].init();
    sensor[sens].configureDefault();
    sensor[sens].writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
    sensor[sens].writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 50);
    sensor[sens].setTimeout(500);
    sensor[sens].stopContinuous();
    delay(300);
    sensor[sens].startInterleavedContinuous(100);
}
