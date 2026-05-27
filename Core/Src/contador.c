#include "main.h"

// ===================== MAPEAMENTO =====================

// Segmentos (A–G + DP) -> GPIOC (PC0–PC7)
#define SEG_PORT GPIOC
#define SEG_A GPIO_PIN_0
#define SEG_B GPIO_PIN_1
#define SEG_C GPIO_PIN_2
#define SEG_D GPIO_PIN_3
#define SEG_E GPIO_PIN_4
#define SEG_F GPIO_PIN_5
#define SEG_G GPIO_PIN_6
#define SEG_DP GPIO_PIN_7

// Displays (transistores) -> GPIOB (PB8–PB11)
#define DIG_PORT GPIOB
#define DIG1 GPIO_PIN_8
#define DIG2 GPIO_PIN_9
#define DIG3 GPIO_PIN_10
#define DIG4 GPIO_PIN_11

// ===================== TABELA (ÂNODO COMUM) =====================
// 0 = segmento ligado | 1 = desligado
const uint8_t seven_seg_lookup[10] = {
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};

// ===================== FUNÇÕES =====================

// desliga todos os displays
void turnOffAllDisplays()
{
    HAL_GPIO_WritePin(DIG_PORT, DIG1 | DIG2 | DIG3 | DIG4, GPIO_PIN_RESET);
}

// liga apenas um display
void enableDisplay(uint8_t pos)
{
    switch (pos)
    {
    case 0:
        HAL_GPIO_WritePin(DIG_PORT, DIG1, GPIO_PIN_SET);
        break;
    case 1:
        HAL_GPIO_WritePin(DIG_PORT, DIG2, GPIO_PIN_SET);
        break;
    case 2:
        HAL_GPIO_WritePin(DIG_PORT, DIG3, GPIO_PIN_SET);
        break;
    case 3:
        HAL_GPIO_WritePin(DIG_PORT, DIG4, GPIO_PIN_SET);
        break;
    }
}

// envia os segmentos
void setSegments(uint8_t value)
{

    HAL_GPIO_WritePin(SEG_PORT, SEG_A, (value & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_PORT, SEG_B, (value & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_PORT, SEG_C, (value & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_PORT, SEG_D, (value & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_PORT, SEG_E, (value & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_PORT, SEG_F, (value & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_PORT, SEG_G, (value & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_PORT, SEG_DP, (value & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// função principal de exibição
void displayNumber(int num)
{

    int d[4];

    d[0] = (num / 1000) % 10;
    d[1] = (num / 100) % 10;
    d[2] = (num / 10) % 10;
    d[3] = num % 10;

    for (int i = 0; i < 4; i++)
    {
        turnOffAllDisplays();                // evita ghosting
        setSegments(seven_seg_lookup[d[i]]); // envia número
        enableDisplay(i);                    // ativa dígito
        HAL_Delay(1);                        // tempo de persistência
    }
}
