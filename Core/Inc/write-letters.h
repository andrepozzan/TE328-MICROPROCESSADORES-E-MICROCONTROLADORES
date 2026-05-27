#ifndef CORE_INC_WRITE_LETTERS_H_
#define CORE_INC_WRITE_LETTERS_H_

#include "main.h"
#include <stdint.h>

// Adicione os tipos de retorno corretos aqui:
void writeLetter(char letter[]);
void scrollText(char texto[]);
uint8_t getColumn(uint8_t mapa[8], int col);
void drawMatrixMap(uint8_t mapa[]);

// Se ColorSelect for um enum do seu projeto, certifique-se que o main.h o define,
// caso contrário, declare a função onde ele fizer sentido:

#endif /* CORE_INC_WRITE_LETTERS_H_ */
