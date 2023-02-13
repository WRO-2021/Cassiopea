#include "movements.h"

/*
 * codice di stefano
 *
 * va bene, e' un casino ma funzione benissimo, e' quello
 * che fa andare bene il robot, ci sono molte costanti
 * trovate a tentativi perche' sia preciso
 */



//the multipliers are choosen after a lot of try
#define GYRO_MULTIPLIER_DX 1.22
#define GYRO_MULTIPLIER_SX 1.15

static float gyro_offset_X;
static float gyro_offset_Y;
static float gyro_offset_Z;

static int mult = 56;  //helou

static float oldAngleX;

float getOldAngleX() {
    return oldAngleX;
}

void move_cm(int cm) {
    motor_set_speed_both(255);
    if (cm > 0) {
        motor_straight();
    } else {
        motor_backward();
    }
    delay(cm * mult);
    motor_break();
}

bool move_cm_avoid_black(int cm) {

    if (oldAngleX < -10) //salita
    {
        //motor_set_speed_both(255);
        motor_set_speed_A(255);
        motor_set_speed_B(205);
        motor_straight();

        //delay(15000);

        float angleX = 0;
        float x, y, z;
        int newT;
        int oldT = millis();
        while (angleX < 10) {
            if (IMU.gyroscopeAvailable()) {
                newT = millis();
                float dt = (newT - oldT) * 1e-3;
                oldT = newT;

                IMU.readGyroscope(x, y, z);
                float gyroX = x + gyro_offset_X;
                angleX += gyroX * dt;
                //Serial.println(angleX);
            }
            if (digitalRead(12) == HIGH) //se il pulsante è premuto //CODICE FELIPRO44
            {
                oldAngleX = 0;
                return true;
            }
        }
        delay(850);

        motor_break();
        oldAngleX = 0;
        return true;
    } else if (oldAngleX > 10) //down the ramp
    {
        motor_set_speed_both(255);
        motor_straight();

        float angleX = 0;
        float x, y, z;
        int newT;
        int oldT = millis();
        while (angleX > -10) {
            if (IMU.gyroscopeAvailable()) {
                newT = millis();
                float dt = (newT - oldT) * 1e-3;
                oldT = newT;

                IMU.readGyroscope(x, y, z);
                float gyroX = x + gyro_offset_X;
                angleX += gyroX * dt;
                //Serial.println(angleX);
            }
            if (digitalRead(12) == HIGH) //se il pulsante è premuto //CODICE FELIPRO44
            {
                oldAngleX = 0;
                return true;
            }
        }
        delay(700);

        motor_break();
        oldAngleX = 0;
        return true;
    } else {
        motor_set_speed_both(255);
        motor_straight();

        float angleX = 0;
        float minAngleX = 0;
        float x, y, z;
        int newT;
        int oldT = millis();

        //black tile organization
        int t0 = millis();
        int t = millis();
        int val = analogRead(REFLEX);
        int c = 0;
        while (t < t0 + cm * mult && (c < 250 || minAngleX < -2)) {
            val = analogRead(REFLEX);
            if (val < BLACK_WHITE_THESHOLD) {
                c++;
            }

            if (IMU.gyroscopeAvailable()) {
                newT = millis();
                float dt = (newT - oldT) * 1e-3;
                oldT = newT;

                IMU.readGyroscope(x, y, z);
                float gyroX = x + gyro_offset_X;
                angleX += gyroX * dt;
                //Serial.println(angleX);
            }

            if (angleX < minAngleX) {
                minAngleX = angleX;
            }
            t = millis();

            if (digitalRead(12) == HIGH) //se il pulsante è premuto //CODICE FELIPRO44
            {
                oldAngleX = 0;
                return true;
            }
        }
        motor_break();

        if (t < t0 + cm * mult) {
            motor_backward();
            delay(t - t0);
            motor_break();
            return false;
        }

        oldAngleX = angleX;
        Serial.println(angleX);
        return true;
    }

}


void gyro_calibrate(int num) {
    float x, y, z;

    gyro_offset_X = 0;
    gyro_offset_Y = 0;
    gyro_offset_Z = 0;

    int i = 0;
    while (i < num) //calibrazione pre-partenza
    {
        if (IMU.gyroscopeAvailable()) {
            IMU.readGyroscope(x, y, z);
            gyro_offset_X -= x;
            gyro_offset_Y -= y;
            gyro_offset_Z -= z;
            i++;
        }
    }

    gyro_offset_X /= num;
    gyro_offset_Y /= num;
    gyro_offset_Z /= num;
}


void turn_degrees(int deg) {

    motor_set_speed_both(255);
    //digitalWrite(LED_BUILTIN, HIGH);

    float angleZ = 0;
    float x, y, z;
    int newT;
    int oldT = millis();

    if (deg > 0) {
        motor_right();
    } else {
        motor_left();
    }

    //stabilisce rotazione dx-sx
    while (deg > 0 ? angleZ * GYRO_MULTIPLIER_DX > -deg : angleZ * GYRO_MULTIPLIER_SX < -deg) {
        if (IMU.gyroscopeAvailable()) {
            newT = millis();
            float dt = (newT - oldT) * 1e-3;
            oldT = newT;

            IMU.readGyroscope(x, y, z);
            float gyroZ = z + gyro_offset_Z;
            angleZ += gyroZ * dt;
            Serial.println(angleZ);
        }
        if (digitalRead(12) == HIGH) //se il pulsante è premuto //CODICE FELIPRO44
        {
            oldAngleX = 0;
            return;
        }
    }

    motor_break();
}

void distanzia_muro() {
    /*
     * in base a che muri ho davanti e dietro
     * vado avanti e indietro un po'
     * voglio che ci sia uguale distanza tra i muri di lato
     * posso alla fine avere un'inclinazione sbagliata
     * ma va bene
     */

    motor_break();
    motor_set_speed_both(72);

    int time0 = millis();

    int t3 = tof_read(3);
    int t4 = tof_read(4);
    int t7 = tof_read(7);
    // se ho dei muri di lato
    if (t3 < 255 && t4 < 255 && t7 < 255) {
        motor_backward();
        // controllo che i muri davanti e dietro siano alla stessa distanza
        while (t3 + tof_get_offset(3) + t4 + tof_get_offset(4) > (t7 + tof_get_offset(7)) * 2) {
            t3 = tof_read(3);
            t4 = tof_read(4);
            t7 = tof_read(7);
            if (digitalRead(12) == HIGH) //se il pulsante è premuto
            {
                oldAngleX = 0;
                return;
            }
            if (millis() - time0 > 5000) {
                motor_break();
                return;
            }
        }
        motor_straight();
        while (t3 + tof_get_offset(3) + t4 + tof_get_offset(4) < (t7 + tof_get_offset(7)) * 2) {
            t3 = tof_read(3);
            t4 = tof_read(4);
            t7 = tof_read(7);
            if (digitalRead(12) == HIGH) //se il pulsante è premuto
            {
                oldAngleX = 0;
                return;
            }
            if (millis() - time0 > 5000) {
                motor_break();
                return;
            }
        }
    } else if (t3 < 255 && t4 < 255) {
        //ho solo un muro dietro

        motor_backward();
        while (t3 + tof_get_offset(3) + t4 + tof_get_offset(4) > 0) {
            t3 = tof_read(3);
            t4 = tof_read(4);
            if (digitalRead(12) == HIGH) //se il pulsante è premuto
            {
                oldAngleX = 0;
                return;
            }
            if (millis() - time0 > 5000) {
                motor_break();
                return;
            }
        }
        motor_straight();
        while (t3 + tof_get_offset(3) + t4 + tof_get_offset(4) < 0) {
            t3 = tof_read(3);
            t4 = tof_read(4);
            if (digitalRead(12) == HIGH) //se il pulsante è premuto
            {
                oldAngleX = 0;
                return;
            }
            if (millis() - time0 > 5000) {
                motor_break();
                return;
            }
        }
    } else if (t7 < 255) {
        // ho solo un muro davanti

        motor_backward();
        while (t7 + tof_get_offset(7) < 0) {
            t7 = tof_read(7);
            if (digitalRead(12) == HIGH) //se il pulsante è premuto
            {
                oldAngleX = 0;
                return;
            }
            if (millis() - time0 > 5000) {
                motor_break();
                return;
            }
        }
        motor_straight();
        while (t7 + tof_get_offset(7) > 0) {
            t7 = tof_read(7);
            if (digitalRead(12) == HIGH) //se il pulsante è premuto
            {
                oldAngleX = 0;
                return;
            }
            if (millis() - time0 > 5000) {
                motor_break();
                return;
            }
        }
    }
    motor_break();
}


void allinea_muro() {
    motor_break();
    motor_set_speed_both(85);//80
    int s0;
    int s1;


    Serial.println("Allineamento muro");
    int mis[8];
    for (int i = 1; i < 7; i++) {
        mis[i] = tof_read(i);
    }

    if (mis[5] < 150 && mis[6] < 150) {
        s0 = 5;
        s1 = 6;
        Serial.println("Muro destra");
    } else if (mis[1] < 150 && mis[2] < 150) {
        s0 = 1;
        s1 = 2;
        Serial.println("Muro sinistra");
    } else if (mis[3] < 150 && mis[4] < 150) {
        s0 = 3;
        s1 = 4;
        Serial.println("Muro dietro");
    } else {
        Serial.println("no muri");
        return;
    }

    int t0 = tof_read(s0) + tof_get_offset(s0);
    int t1 = tof_read(s1) + tof_get_offset(s1);

    int temp0;
    int temp1;

    int time0 = millis();;

    motor_left();
    // giro a sinistra finchè non sono parallelo al muro
    while (t1 < t0) {
        temp0 = tof_read(s0);
        temp1 = tof_read(s1);

        // se sbatto contro qualcosa o vado troppo lontano mi fermo
        if (temp0 == 255 || temp1 == 255 || temp0 == -1 || temp1 == -1) {
            motor_break();
            return;
        }

        t0 = temp0 + tof_get_offset(s0);
        t1 = temp1 + tof_get_offset(s1);

        if (digitalRead(12) == HIGH) //se il pulsante è premuto
        {
            oldAngleX = 0;
            return;
        }

        if (millis() - time0 > 5000) {
            motor_break();
            return;
        }
    }

    motor_right();
    // idem a destra
    while (t1 > t0/* && tof_read(s0)!= 255 && tof_read(s1)!= 255*/) {
        temp0 = tof_read(s0);
        temp1 = tof_read(s1);

        if (temp0 == 255 || temp1 == 255 || temp0 == -1 || temp1 == -1) {
            motor_break();
            return;
        }

        t0 = temp0 + tof_get_offset(s0);
        t1 = temp1 + tof_get_offset(s1);

        if (digitalRead(12) == HIGH) //se il pulsante è premuto //CODICE FELIPRO44
        {
            oldAngleX = 0;
            return;
        }
        {
            motor_break();
            return;
        }
    }

    motor_break();
}


void allinea_muro_bene() {
    motor_break();
    motor_set_speed_both(80);//72
    int s0;
    int s1;


    Serial.println("Allineamento muro");

    int mis[8];
    for (int i = 1; i < 7; i++) {
        mis[i] = tof_read(i);
    }

    int dis; // distanza in mm dal muro di destra relativa al centro
    if (mis[5] < 150 && mis[6] < 150) {
        s0 = 5;
        s1 = 6;
        dis = (mis[5] + tof_get_offset(5) + mis[6] + tof_get_offset(6)) / 2;
        Serial.println("Muro destra");
    } else if (mis[1] < 150 && mis[2] < 150) {
        s0 = 1;
        s1 = 2;
        dis = -(mis[1] + tof_get_offset(1) + mis[2] + tof_get_offset(2)) / 2;
        Serial.println("Muro sinistra");
    } else if (mis[3] < 150 && mis[4] < 150) {
        allinea_muro();
        Serial.println("Muro dietro");
    } else {
        Serial.println("no muri");
        return;
    }

    int t0 = tof_read(s0) + tof_get_offset(s0);
    int t1 = tof_read(s1) + tof_get_offset(s1);

    motor_left();
    while (t1 < t0 && tof_read(s0) != 255 && tof_read(s1) != 255) {
        t0 = tof_read(s0) + tof_get_offset(s0);
        t1 = tof_read(s1) + tof_get_offset(s1) + 11.7 * dis / 30;//  // distanza_tof:x = tyle:dis
    }

    motor_right();
    while (t1 > t0 && tof_read(s0) != 255 && tof_read(s1) != 255) {
        t0 = tof_read(s0) + tof_get_offset(s0);
        t1 = tof_read(s1) + tof_get_offset(s1) + 11.7 * dis / 30;
    }

    motor_break();
}
