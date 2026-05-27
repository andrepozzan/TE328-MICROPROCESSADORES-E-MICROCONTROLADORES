/*
 * leds.c
 *
 *  Created on: Mar 11, 2026
 *      Author: mvhra
 */
#include "main.h"
#include "alfabeto.h"

int dados_7seg[4] = {0};

void Apaga_Matriz(void)
{
	// Apaga tudo
	GPIOC->ODR |= 0x00FF;
	GPIOB->ODR &= 0x00FF;
}

void Testa_Matriz(void)
{
	// Varre as linhas
	for (int i = 0; i < 8; i++)
	{
		// Desseleciona todas as linhas
		GPIOC->ODR |= 0x00FF;

		// Seleciona linha i
		GPIOC->ODR &= ~(1 << i);

		// Varre as colunas
		for (int j = 0; j < 8; j++)
		{
			// Apaga todas as colunas
			GPIOB->ODR &= 0x00FF;

			// Acende a coluna j
			GPIOB->ODR |= 1 << (j + 8);

			HAL_Delay(100);
		}
	}

	Apaga_Matriz();
}

void Cfg_PC8_PC11(void)
{
	// Zera os bits de modo dos pinos 8, 9, 10 e 11
	GPIOC->MODER &= 0XFF00FFFF;

	// Configura os bits de modo dos pinos 8, 9, 10 e 11
	GPIOC->MODER |= 0x01 << (2 * 8) |
					0x01 << (2 * 9) |
					0x01 << (2 * 10) |
					0x01 << (2 * 11);
}

void TurnOff_7seg(void)
{
	// Desseleciona todos os displays
	GPIOC->ODR &= ~(0x0F00); // PC8 a PC11 = 0

	// Apaga todas os segmentos
	GPIOB->ODR &= 0x00FF;
}

void Test_7Seg(void)
{
	// Varre os displays
	for (int i = 0; i < 4; i++)
	{
		// Desseleciona todos os displays
		GPIOC->ODR &= ~(0x0F00); // PC8 a PC11 = 0

		// Seleciona o display i
		GPIOC->ODR |= 1 << (i + 8);

		// Varre os segmentos
		for (int j = 0; j < 8; j++)
		{
			// Apaga todas os segmentos
			GPIOB->ODR &= 0x00FF;

			// Liga o segmento j
			GPIOB->ODR |= 1 << (j + 8);

			HAL_Delay(200);
		}
	}

	TurnOff_7seg();
}

void Display_7Seg(int valor, int posicao, int dp)
{
	// Apaga os displays de 7 segmentos
	GPIOC->ODR &= 0xF0FF;

	// Seleciona o display de 7 segmentos, corrigindo a inversao da placa
	if (posicao == 0)
		GPIOC->ODR |= 0x0100;
	else if (posicao == 1)
		GPIOC->ODR |= 0x0800;
	else if (posicao == 2)
		GPIOC->ODR |= 0x0400;
	else if (posicao == 3)
		GPIOC->ODR |= 0x0200;

	// Mostra o valor no display de 7 segmentos selecionado
	GPIOB->ODR &= 0x00FF;
	GPIOB->ODR |= decod_7_seg[valor] << 8;
	GPIOB->ODR |= (dp & 0x01) << 15;
}

void Print_7Seg(int valor)
{										// Separa o valor inteiro nas suas casas decimais
	dados_7seg[0] = valor % 10;			// unidade
	dados_7seg[1] = (valor / 10) % 10;	// dezena
	dados_7seg[2] = (valor / 100) % 10; // centena
	dados_7seg[3] = valor / 1000;		// milhar
}

int posicao_LEDs = 0;

void Refresh_LEDs(void)
{
	Display_7Seg(dados_7seg[posicao_LEDs], posicao_LEDs, 0);
	if (++posicao_LEDs >= 4)
		posicao_LEDs = 0;
}
