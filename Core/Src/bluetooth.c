#include "bluetooth.h"
#include <stdio.h>
#include <string.h>

extern TIM_HandleTypeDef htim2;

void bluetoothCommand(char *cmd, volatile uint8_t *r, volatile uint8_t *g, volatile uint8_t *b, volatile uint8_t *update_flag)
{
    char color;
    int value;

    // Tenta extrair o formato "Caractere = Inteiro"
    if (sscanf(cmd, " %c = %d", &color, &value) >= 2)
    {
        // Limita o valor entre 0 e 100
        if (value < 0)
            value = 0;
        if (value > 100)
            value = 100;

        // Atualiza a variável e o hardware (PWM) de acordo com a cor
        switch (color)
        {
        case 'R':
        case 'r':
            *r = (uint8_t)value;
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, *r);
            break;
        case 'G':
        case 'g':
            *g = (uint8_t)value;
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, *g);
            break;
        case 'B':
        case 'b':
            *b = (uint8_t)value;
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, *b);
            break;
        default:
            return; // Comando inválido
        }

        // Sinaliza para a função principal atualizar o display OLED
        *update_flag = 1;
    }
}
