#include "square2.h"
#include "main.h"

// Protótipo da sua função existente para o compilador não reclamar
void drawMatrixMap(const uint8_t mapa[]);

void Draw_Square_2x2(uint8_t x, uint8_t y)
{
    // 1. Limita coordenadas (x e y devem estar entre 0 e 6)
    if (x > 6)
        x = 6;
    if (y > 6)
        y = 6;

    uint8_t mapa_quadrado[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    // 2. Lógica de deslocamento robusta
    // Em vez de calcular o deslocamento na hora, vamos definir o padrão 2x2 fixo
    // e apenas deslocá-lo. 0xC0 é '11000000'.
    // Ao mover para a direita (x aumenta), deslocamos os bits para a direita.
    uint8_t bits_linha = (0xC0 >> x);

    // 3. Aplica nas linhas
    mapa_quadrado[y] = bits_linha;
    mapa_quadrado[y + 1] = bits_linha;

    // 4. Varredura
    for (int i = 0; i < 3; i++)
    {
        drawMatrixMap(mapa_quadrado);
    }
}