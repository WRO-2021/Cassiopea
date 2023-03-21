#include "motors.h"
#include "movements.h"
#include "exploration.h"
#include "tof.h"
#include "kit.h"
#include "maze.h"

maze campo;

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

    Wire.begin();
    Serial.begin(115200);
    //while (!Serial);
    Serial.println("Started");

    Serial.println("TOFs initialization");
    for(int i=1; i<8; i++)
        tof.get_instance(i);


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
        int dis = tof.get_instance(i).read();
        if (dis == -1) tof.get_instance(i).conf();
    }

    Serial.println("Setup done");
    digitalWrite(LED_BUILTIN, LOW);


    ////////////////////////////////

    // set serve kit
    servo_kit.attach(5);
    servo_kit.write(95);

    campo = maze();

    allinea_muro();
    distanzia_muro();

    esplora();
}

bool finito = false;

void loop() {
    while (!finito) {
        if (digitalRead(12) == HIGH) {
            // pulsante lack of progress
            motor_break();
            digitalWrite(2, HIGH);
            delay(2000); //pausa di sicurezza (no doppi click ecc...)

            while (digitalRead(12) == LOW); //attendo che il pulsante venga rischiacciato

            digitalWrite(2, LOW);
            delay(1800); //pausa di sicurezza (non parte mentre si sta ancora toccando)

            digitalWrite(2, HIGH);
            delay(200);
            digitalWrite(2, LOW);

            maze.goto_checkpoint();
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
