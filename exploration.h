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

int absolute_dir_to_relative(int abs);

int relative_dir_to_absolute(int rel);

bool muro_abs(int abs);
bool muro_rel(int rel);
int ix(int direction);
int iy(int direction);

void scan_neighbors();

void found_victim(int kits);

void check_for_victims();

void gira_destra();
void gira_sinistra();
void gira_180();
void avanti();

void gira(int direction);

bool priority_path_to(char dest);

bool esplora();


#endif
