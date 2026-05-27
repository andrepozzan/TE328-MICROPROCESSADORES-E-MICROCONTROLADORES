#include "main.h"
#include <stdlib.h>
#include "stm32g4xx_hal.h"
#include <stdio.h>

void apagaMatrix(void)
{
    GPIOC->ODR |= 0x00FF;
    GPIOB->ODR &= ~(0xFF << 8);
}

void turnOnLed(int x, int y)
{
    GPIOC->ODR |= 0x00FF;
    GPIOB->ODR &= ~(0xFF << 8);

    GPIOC->ODR &= ~(1 << x);      // ativa linha
    GPIOB->ODR |= (1 << (y + 8)); // ativa coluna
}

void testMatrix(void)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            turnOnLed(i, j);
            HAL_Delay(50); // mais rápido
        }
    }
    apagaMatrix();
}
