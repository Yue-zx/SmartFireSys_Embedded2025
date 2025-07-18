#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

	void TIM15_PWM_Start(void);
	void TIM15_PWM_SetCompare1(uint16_t Compare);
	void TIM15_PWM_SetCompare2(uint16_t Compare);

#ifdef __cplusplus
}
#endif
#endif
