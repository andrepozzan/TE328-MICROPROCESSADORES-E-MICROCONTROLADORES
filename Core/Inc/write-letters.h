#ifndef CORE_INC_WRITE_LETTERS_H_
#define CORE_INC_WRITE_LETTERS_H_

#include "main.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // Adicione os tipos de retorno corretos aqui:
    void writeLetter(const char *letter);
    void scrollText(const char *texto);
    uint8_t getColumn(const uint8_t mapa[8], int col);
    void drawMatrixMap(const uint8_t mapa[]);
    const uint8_t *getLetterMap(char letter);

    // Se ColorSelect for um enum do seu projeto, certifique-se que o main.h o define,
    // caso contrário, declare a função onde ele fizer sentido:

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_WRITE_LETTERS_H_ */
