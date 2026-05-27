#ifndef INC_RGB_H_
#define INC_RGB_H_

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void Atualiza_Displays_Unificado(void);
    void Atualizar_PWM(void);
    void Atualizar_Matriz_8x8(ColorSelect cor);

#ifdef __cplusplus
}
#endif

#endif /* INC_RGB_H_ */
