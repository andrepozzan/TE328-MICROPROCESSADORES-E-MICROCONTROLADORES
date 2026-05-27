#ifndef SQUARE2_H
#define SQUARE2_H

#include <stdint.h>

/**
 * @brief Desenha um quadrado 2x2 na matriz de LEDs.
 * @param x Posição X da origem (canto superior esquerdo) - Limite recomendado: 0 a 6
 * @param y Posição Y da origem (canto superior esquerdo) - Limite recomendado: 0 a 6
 */
void Draw_Square_2x2(uint8_t x, uint8_t y);

#endif /* SQUARE2_H */