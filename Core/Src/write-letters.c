#include "main.h"
#include <stdlib.h>
#include "stm32g4xx_hal.h"
#include <stdio.h>
#include "write-letters.h"

void writeLetter(char letter[]);
void scrollText(char texto[]);
uint8_t getColumn(uint8_t mapa[8], int col);
void rotateAntiClockwise(uint8_t src[8], uint8_t dst[8]);

typedef struct
{
    char caractere;
    uint8_t bitmap[8];
} FonteMatriz;

FonteMatriz characters[] = {
    {'T', {0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}},
    {'E', {0xFF, 0xC0, 0xC0, 0xFC, 0xC0, 0xC0, 0xFF, 0x00}},
    {'2', {0x7E, 0xC3, 0x03, 0x0E, 0x38, 0x60, 0xFF, 0x00}},
    {'0', {0x7E, 0xC3, 0xDB, 0xDB, 0xDB, 0xC3, 0x7E, 0x00}},
    {'6', {0x3E, 0x60, 0xC0, 0xFE, 0xC3, 0xC3, 0x7E, 0x00}},
    {'3', {0x7E, 0x03, 0x03, 0x3E, 0x03, 0x03, 0x7E, 0x00}},
    {'8', {0x7E, 0xC3, 0xC3, 0x7E, 0xC3, 0xC3, 0x7E, 0x00}},
    {'U', {0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0x7E, 0x00}},
    {'R', {0xFE, 0xC3, 0xC3, 0xFE, 0xD8, 0xCC, 0xC6, 0x00}},
    {'M', {0xC3, 0xE7, 0xFF, 0xDB, 0xC3, 0xC3, 0xC3, 0x00}},
    {'A', {0x3C, 0x66, 0xC3, 0xFF, 0xC3, 0xC3, 0xC3, 0x00}},
    {'-', {0x00, 0x00, 0x00, 0x3C, 0x3C, 0x00, 0x00, 0x00}},
    {'G', {0x3E, 0x63, 0xC0, 0xCF, 0xC3, 0x63, 0x3E, 0x00}},
    {'o', {0x00, 0x00, 0x7E, 0xC3, 0xC3, 0xC3, 0x7E, 0x00}},
    {'B', {0xFC, 0xC6, 0xC6, 0xFC, 0xC6, 0xC6, 0xFC, 0x00}},
};

void drawMatrixMap(uint8_t mapa[])
{
    for (int i = 0; i < 8; i++)
    {
        int linha = 7 - i;

        GPIOC->ODR |= 0x00FF;
        GPIOB->ODR &= ~(0xFF << 8);

        uint16_t colunas = (uint16_t)mapa[linha] << 8;

        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | colunas;

        GPIOC->ODR &= ~(1 << i);

        HAL_Delay(1);
    }
}

uint8_t *getMap(char c)
{
    int tamanho = sizeof(characters) / sizeof(FonteMatriz);
    for (int i = 0; i < tamanho; i++)
    {
        if (characters[i].caractere == c)
        {
            return characters[i].bitmap;
        }
    }
    return NULL;
}

void writeLetter(char letter[])
{
    for (int i = 0; letter[i] != '\0'; i++)
    {
        uint8_t *mapa = getMap(letter[i]);
        if (mapa != NULL)
        {
            for (int t = 0; t < 100; t++)
            {
                drawMatrixMap(mapa);
            }
        }
    }
}

uint8_t getColumn(uint8_t mapa[8], int col)
{
    uint8_t coluna = 0;
    for (int i = 0; i < 8; i++)
    {
        if (mapa[i] & (1 << (7 - col)))
        {
            coluna |= (1 << i);
        }
    }
    return coluna;
}

void rotateAntiClockwise(uint8_t src[8], uint8_t dst[8])
{
    for (int i = 0; i < 8; i++)
    {
        dst[i] = 0;
        for (int j = 0; j < 8; j++)
        {
            if (src[j] & (1 << i))
            {
                dst[i] |= (1 << (7 - j));
            }
        }
    }
}

void scrollText(char texto[])
{
    uint8_t buffer[200];
    int pos = 0;

    for (int k = 0; texto[k] != '\0'; k++)
    {
        uint8_t *mapa = getMap(texto[k]);
        if (mapa != NULL)
        {
            for (int c = 0; c < 8; c++)
            {
                buffer[pos++] = getColumn(mapa, c);
            }
            buffer[pos++] = 0x00;
        }
    }

    for (int offset = 0; offset < pos - 8; offset++)
    {
        uint8_t frame[8];
        for (int i = 0; i < 8; i++)
        {
            frame[i] = buffer[offset + i];
        }

        uint8_t frameRot[8];
        rotateAntiClockwise(frame, frameRot);

        for (int t = 0; t < 5; t++)
        {
            drawMatrixMap(frameRot);
        }
    }
}
