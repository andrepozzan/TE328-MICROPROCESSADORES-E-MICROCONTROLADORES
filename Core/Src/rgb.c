/*
 * rgb.c
 *
 * Created on: May 20, 2026
 * Author: Aluno
 */

#include "rgb.h"
#include "alfabeto.h"
#include "write-letters.h"

extern float red_duty_cycle;
extern float green_duty_cycle;
extern float blue_duty_cycle;
extern ColorSelect selected_color;
extern uint8_t scan_step;
extern TIM_HandleTypeDef htim2;

void updateMatrix8x8(ColorSelect cor)
{
    if (cor == RED)
    {
        writeLetter("R");
    }
    else if (cor == GREEN)
    {
        writeLetter("G");
    }
    else
    {
        writeLetter("B");
    }
}

void updateUnifiedDisplays(void)
{
    float focused_value = 0.0f;
    const uint8_t *matrix_image = NULL;
    char letter = 'R';

    if (selected_color == RED)
    {
        focused_value = red_duty_cycle;
        letter = 'R';
    }
    else if (selected_color == GREEN)
    {
        focused_value = green_duty_cycle;
        letter = 'G';
    }
    else
    {
        focused_value = blue_duty_cycle;
        letter = 'B';
    }

    matrix_image = getLetterMap(letter);
    if (matrix_image == NULL)
    {
        return;
    }

    int intensity = (int)(focused_value * 10.0f);

    int digit_1 = (intensity / 1000) % 10;
    int digit_2 = (intensity / 100) % 10;
    int digit_3 = (intensity / 10) % 10;
    int digit_4 = intensity % 10;

    HAL_GPIO_WritePin(GPIOC, 0x00FF, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);

    if (scan_step < 8)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (matrix_image[7 - scan_step] << 8);
        HAL_GPIO_WritePin(GPIOC, (1 << scan_step), GPIO_PIN_RESET);
    }
    else if (scan_step == 8)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (seven_seg_patterns[digit_1] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    }
    else if (scan_step == 9)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (seven_seg_patterns[digit_2] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
    }
    else if (scan_step == 10)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | ((seven_seg_patterns[digit_3] | 0x80) << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
    }
    else if (scan_step == 11)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (seven_seg_patterns[digit_4] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    }

    scan_step++;
    if (scan_step > 11)
    {
        scan_step = 0;
    }
}

void updatePwm(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (uint32_t)(red_duty_cycle * 9.99f));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (uint32_t)(green_duty_cycle * 9.99f));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, (uint32_t)(blue_duty_cycle * 9.99f));
}
