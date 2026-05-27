#ifndef INC_RGB_H_
#define INC_RGB_H_

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void updateUnifiedDisplays(void);
    void updatePwm(void);
    void updateMatrix8x8(ColorSelect cor);

#ifdef __cplusplus
}
#endif

#endif /* INC_RGB_H_ */
