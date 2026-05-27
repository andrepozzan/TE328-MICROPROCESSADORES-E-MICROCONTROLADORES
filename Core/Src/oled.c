#include "oled.h"
#include "ssd1306.h"
#include "fonts.h"
#include <stdio.h>

// Dentro de oled.c e bluetooth.c

void oledDisplayInit(void)
{
    SSD1306_Init();
    SSD1306_Clear();
    SSD1306_UpdateScreen();
}

void updateOLEDDisplay(uint8_t r, uint8_t g, uint8_t b)
{
    char buffer[20];

    SSD1306_Clear();

    sprintf(buffer, "RED  : %d %%", r);
    SSD1306_GotoXY(10, 5);
    SSD1306_Puts(buffer, &Font_7x10, SSD1306_COLOR_WHITE);

    sprintf(buffer, "GREEN: %d %%", g);
    SSD1306_GotoXY(10, 25);
    SSD1306_Puts(buffer, &Font_7x10, SSD1306_COLOR_WHITE);

    sprintf(buffer, "BLUE : %d %%", b);
    SSD1306_GotoXY(10, 40);
    SSD1306_Puts(buffer, &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_UpdateScreen();
}