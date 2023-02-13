#ifndef exploration_h
#define exploration_h

#include <Arduino.h>
#include <Adafruit_MLX90614.h>

#include <Adafruit_TCS34725.h>
#include "motors.h"
#include "movements.h"
#include "tof.h"
#include "kit.h"

#define LED_R 22
#define LED_G 23
#define LED_B 24


void campo_init();
void campo_stampa();

bool muro_nord();
bool muro_est();
bool muro_sud();
bool muro_ovest();

void scan_neighbors();
void found_victim(int kits);
void check_for_victims();

void gira_destra();
void gira_sinistra();
void gira_180();
void avanti();

bool priority_path_to(char dest);
bool priority_path_to(int destx, int desty);

bool esplora();



#endif
