
#define INC_LEDS_H_

#include <stdint.h>

#include "main.h" // Importante para o HAL_Status e tipos uint8_t

// Protótipos das funções
void processaJoystick(void);
void renderizaCena(uint8_t mapa[]);

void testMatrix(void);

#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

// Variáveis globais do player
extern int playerX;
extern int playerY;

// Funções para o display LED
void clearMatrix(void);
void turnOnLed(int x, int y);
void testMatrix(void);
void atualizaMapa(uint8_t mapa[], int px, int py);
void desenhaCena(uint8_t mapa[]);
void atualizaPlayerJoystick(void);

#endif /*INC_LEDS_H_*/
