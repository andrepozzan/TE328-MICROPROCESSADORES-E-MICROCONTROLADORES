#include "square2.h"
#include "main.h"

// Protótipo da sua função existente para o compilador não reclamar
void drawMatrixMap(const uint8_t mapa[]);

void Draw_Square_2x2(uint8_t x, uint8_t y)
{
    // Limita as coordenadas para garantir que o quadrado de 2x2 não "estoure" a matriz 8x8
    if (x > 6)
        x = 6;
    if (y > 6)
        y = 6;

    // Cria um mapa de tela em branco (todos os LEDs apagados)
    uint8_t mapa_quadrado[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // No seu mapa, o bit mais significativo (MSB, 0x80) representa o lado esquerdo.
    // Portanto, deslocamos os bits para a esquerda baseado na coordenada X.
    uint8_t bits_linha = (0x03 << (6 - x));

    // O quadrado ocupa duas linhas consecutivas a partir da posição Y
    mapa_quadrado[y] = bits_linha;     // Linha superior do quadrado
    mapa_quadrado[y + 1] = bits_linha; // Linha inferior do quadrado

    // Chama a sua função nativa que controla os registradores GPIOB/GPIOC por multiplexação
    // Como a varredura interna dela demora 8ms (1ms por linha), chamamos poucas vezes para não travar o ADC
    for (int i = 0; i < 3; i++)
    {
        drawMatrixMap(mapa_quadrado);
    }
}