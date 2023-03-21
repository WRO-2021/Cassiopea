/**
 * source: https://github.com/pololu/vl6180x-arduino/blob/master/examples/InterleavedContinuous/InterleavedContinuous.ino
 *
**/

#include "tof.h"


static int tof::tof_offset[] = {
        0,//il primo è scollegato
        -93,
        -97,
        -65,
        -63,
        -94,
        -102,
        -75
};

static vector<tof> tof::instances(8);

static tof tof::get_instance(uint8_t index) {
    if (instances[index].sensor == nullptr) {
        instances[index] = tof(index);
    }
    return instances[index];
}

tof::tof(uint8_t bus) {
    this->bus = bus;
    this->sensor = VL6180X();
    conf();
}

void tof::conf() {
    mux()

    sensor.init();
    sensor.configureDefault();
    sensor.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 30);
    sensor.writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 50);
    sensor.setTimeout(500);
    sensor.stopContinuous();
    delay(300);
    sensor[sens].startInterleavedContinuous(300);
}

void tof::mux() {
    //set the transmission with the MUX
    Wire.beginTransmission(0x70);
    Wire.write(1 << bus);
    Wire.endTransmission();
}

int tof::read() {
    mux();
    int ret = sensor.readRangeContinuousMillimeters();
    if (sensor.timeoutOccurred()) {
        // sensore fa timeout, prova a riconfigurarlo, non e' una vera soluzione
        conf();
        ret = sensor.readRangeContinuousMillimeters();
        if (sensor.timeoutOccurred())
            return -1;
    }
    return ret;
}

int tof::read_with_offset() {
    int ret = read();
    if (ret == -1)
        return -1;
    return ret + tof_offset[bus];
}

static tof tof::get_in_direction(int direction, int num) {
    switch (direction) {
        case 0:
            return get_instance(7);
        case 1:
            return get_instance(num ? 5 : 6);
        case 2:
            return get_instance(num ? 4 : 3);
        case 3:
            return get_instance(num ? 2 : 1);
    }
}