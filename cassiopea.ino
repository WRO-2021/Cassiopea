#include "motors.h"
#include "movements.h"
#include "exploration.h"


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(12, INPUT); //button
    pinMode(2, OUTPUT); //led lack of progress
    pinMode(3, OUTPUT); //led victim
    pinMode(LED_R, OUTPUT); //led r
    pinMode(LED_G, OUTPUT); //led g
    pinMode(LED_B, OUTPUT); //led b

    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);

    pinMode(REFLEX, INPUT);

    Wire.begin();
    Serial.begin(115200);
    //while (!Serial);
    Serial.println("Started");

    Serial.println("TOFs initialization");
    tof_conf_all();

    Serial.println("IMU initialization");
    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU!");
        while (1);
    }
    Serial.println("Gyroscope calibration... do not move the IMU");
    gyro_calibrate(500);

    Serial.println("Motors initialization");
    motor_init();
    motor_set_speed_both(192);

    //tof check
    Serial.println("tof check");
    for (int i = 1; i < 8; i++) {
        int dis = tof_read(i);
        if (dis == -1) {
            tof_conf(i);
        }
    }

    Serial.println("Setup done");
    digitalWrite(LED_BUILTIN, LOW);


    ////////////////////////////////

    tcs.begin();//color
    mlx.begin();//temperature
    // set serve kit
    servo_kit.attach(5);
    servo_kit.write(95);

    campo_init();

    allinea_muro();
    distanzia_muro();

    esplora();
}

bool finito = false;
void loop() {
    while (!finito) {
        if (digitalRead(12)){


            //in case of lack of progress
            digitalWrite(LED_BUILTIN, LOW);
            esplora();
        }

    }

    // in esplora esce se schiaccio il pulsante


    // lampeggio led, lampeggia quando torna ha finito di esplorare
    if (millis() % 1000 < 500) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }

    campo_stampa();
}
