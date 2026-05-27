/*
 * rgb.c
 *
 * Created on: May 20, 2026
 * Author: Aluno
 */

#include "rgb.h"
#include "alfabeto.h"
#include "write-letters.h"

extern float duty_R;
extern float duty_G;
extern float duty_B;
extern ColorSelect cor_atual;
extern uint8_t passo_varredura;
extern TIM_HandleTypeDef htim2;

void Atualizar_Matriz_8x8(ColorSelect cor)
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

void Atualiza_Displays_Unificado(void)
{
    float valor_foco = 0.0f;
    const uint8_t *imagem_matriz = NULL;
    char letra = 'R';

    if (cor_atual == RED)
    {
        valor_foco = duty_R;
        letra = 'R';
    }
    else if (cor_atual == GREEN)
    {
        valor_foco = duty_G;
        letra = 'G';
    }
    else
    {
        valor_foco = duty_B;
        letra = 'B';
    }

    imagem_matriz = getLetterMap(letra);
    if (imagem_matriz == NULL)
    {
        return;
    }

    int intensidade = (int)(valor_foco * 10.0f);

    int digito_1 = (intensidade / 1000) % 10;
    int digito_2 = (intensidade / 100) % 10;
    int digito_3 = (intensidade / 10) % 10;
    int digito_4 = intensidade % 10;

    HAL_GPIO_WritePin(GPIOC, 0x00FF, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);

    if (passo_varredura < 8)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (imagem_matriz[7 - passo_varredura] << 8);
        HAL_GPIO_WritePin(GPIOC, (1 << passo_varredura), GPIO_PIN_RESET);
    }
    else if (passo_varredura == 8)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (decod_7_seg[digito_1] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    }
    else if (passo_varredura == 9)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (decod_7_seg[digito_2] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
    }
    else if (passo_varredura == 10)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | ((decod_7_seg[digito_3] | 0x80) << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
    }
    else if (passo_varredura == 11)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (decod_7_seg[digito_4] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    }

    passo_varredura++;
    if (passo_varredura > 11)
    {
        passo_varredura = 0;
    }
}

void Atualizar_PWM(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (uint32_t)(duty_R * 9.99f));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (uint32_t)(duty_G * 9.99f));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, (uint32_t)(duty_B * 9.99f));
}
