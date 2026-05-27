#include "main.h"
#include "alfabeto.h"

int seven_seg_digits[4] = {0};

void configurePc8ToPc11(void)
{
	// Zera os bits de modo dos pinos 8, 9, 10 e 11
	GPIOC->MODER &= 0XFF00FFFF;

	// Configura os bits de modo dos pinos 8, 9, 10 e 11
	GPIOC->MODER |= 0x01 << (2 * 8) |
					0x01 << (2 * 9) |
					0x01 << (2 * 10) |
					0x01 << (2 * 11);
}

void turnOff7Seg(void)
{
	// Desseleciona todos os displays
	GPIOC->ODR &= ~(0x0F00); // PC8 a PC11 = 0

	// Apaga todas os segmentos
	GPIOB->ODR &= 0x00FF;
}

void test7Seg(void)
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

	turnOff7Seg();
}

void display7Seg(int value, int position, int decimalPoint)
{
	// Apaga os displays de 7 segmentos (Controle dos catodos/anodos no Port C)
	GPIOC->ODR &= 0xF0FF;

	// Seleciona o display de 7 segmentos
	if (position == 0)
		GPIOC->ODR |= 0x0100;
	else if (position == 1)
		GPIOC->ODR |= 0x0800;
	else if (position == 2)
		GPIOC->ODR |= 0x0400;
	else if (position == 3)
		GPIOC->ODR |= 0x0200;

	// --- CORREÇÃO BITWISE PARA O PORT B ---
	// Em vez de limpar tudo, limpamos apenas os bits que pertencem aos segmentos do display.
	// Supondo que você use de PB8 a PB15, a máscara isola esses pinos salvando o resto.
	uint32_t temp_odr = GPIOB->ODR;
	temp_odr &= 0x00FF;							  // Limpa apenas a parte alta onde ficam os segmentos
	temp_odr |= (seven_seg_patterns[value] << 8); // Insere o caractere decodificado
	temp_odr |= ((decimalPoint & 0x01) << 15);	  // Insere o ponto decimal se houver

	GPIOB->ODR = temp_odr; // Aplica de volta mantendo os pinos inferiores intactos
}

void prepare7SegDigits(int value)
{											  // Separa o value inteiro nas suas casas decimais
	seven_seg_digits[0] = value % 10;		  // unidade
	seven_seg_digits[1] = (value / 10) % 10;  // dezena
	seven_seg_digits[2] = (value / 100) % 10; // centena
	seven_seg_digits[3] = value / 1000;		  // milhar
}

int display_scan_index = 0;

void refreshSevenSegDisplays(void)
{
	display7Seg(seven_seg_digits[display_scan_index], display_scan_index, 0);
	if (++display_scan_index >= 4)
		display_scan_index = 0;
}
