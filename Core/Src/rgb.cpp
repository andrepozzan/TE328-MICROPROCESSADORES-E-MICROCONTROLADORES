/*
 * rgb.c
 *
 * Created on: May 20, 2026
 * Author: Aluno
 */

#include "rgb.h"
#include "main.h"

extern float duty_R;
extern float duty_G;
extern float duty_B;
extern int cor_atual;
extern uint8_t passo_varredura;
extern TIM_HandleTypeDef htim2;

// Tabelas de decodificação de hardware
extern const uint8_t display_7seg[10];
extern const uint8_t matriz_R[8];
extern const uint8_t matriz_G[8];
extern const uint8_t matriz_B[8];


void Atualizar_Matriz_8x8(ColorSelect cor){
    if(cor == RED) writeLetter("R");
    else if (cor == GREEN) writeLetter("G");
    else writeLetter("B");
}

void Atualiza_Displays_Unificado(void)
{
    float valor_foco = 0.0;
    const uint8_t *imagem_matriz;

    // 0 = RED, 1 = GREEN, 2 = BLUE
    if (cor_atual == 0) {
        valor_foco = duty_R;
        imagem_matriz = matriz_R;
    } else if (cor_atual == 1) {
        valor_foco = duty_G;
        imagem_matriz = matriz_G;
    } else {
        valor_foco = duty_B;
        imagem_matriz = matriz_B;
    }

    // Converte float (0.0 a 100.0) para inteiro com casa decimal (0 a 1000)
    int intensidade = (int)(valor_foco * 10.0f);

    int digito_1 = (intensidade / 1000) % 10; // Centena
    int digito_2 = (intensidade / 100) % 10;  // Dezena
    int digito_3 = (intensidade / 10) % 10;   // Unidade
    int digito_4 = intensidade % 10;          // Casa Decimal

    // Apaga a Matriz (PC0 ao PC7) -> PNP desliga em HIGH
    HAL_GPIO_WritePin(GPIOC, 0x00FF, GPIO_PIN_SET);
    // Apaga os Displays (PC8 ao PC11) -> NPN desliga em LOW
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);

    // --- EXECUTA A FATIA DA VEZ (MULTIPLEXAÇÃO) ---
    if (passo_varredura < 8)
    {
        // Atualiza barramento de dados PB8..PB15 (Mantém PB0..PB7 intactos)
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (imagem_matriz[7 - passo_varredura] << 8);
        // Liga a linha correspondente da matriz (Ativa PNP com LOW)
        HAL_GPIO_WritePin(GPIOC, (1 << passo_varredura), GPIO_PIN_RESET);
    }
    else if (passo_varredura == 8)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (display_7seg[digito_1] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // DIG1
    }
    else if (passo_varredura == 9)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (display_7seg[digito_2] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET); // DIG2
    }
    else if (passo_varredura == 10)
    {
        // Inclui o Ponto Decimal fixo na unidade (Bit 7 do display aceso -> 0x80)
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | ((display_7seg[digito_3] | 0x80) << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); // DIG3
    }
    else if (passo_varredura == 11)
    {
        GPIOB->ODR = (GPIOB->ODR & 0x00FF) | (display_7seg[digito_4] << 8);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // DIG4
    }

    // Incrementa o passo ciclicamente
    passo_varredura++;
    if (passo_varredura > 11) {
        passo_varredura = 0;
    }
}

void Atualizar_PWM(void)
{
    // Converte a escala de 0.0-100.0% para a escala do ARR do Timer (0-999)
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (uint32_t)(duty_R * 9.99f));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (uint32_t)(duty_G * 9.99f));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, (uint32_t)(duty_B * 9.99f));
}
