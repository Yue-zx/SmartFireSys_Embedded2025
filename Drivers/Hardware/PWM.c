#include "PWM.h"
#include "tim.h"

void TIM15_PWM_Start(void)
{
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);
}

void TIM15_PWM_SetCompare1(uint16_t Compare)  
{
	__HAL_TIM_SetCompare(&htim15,TIM_CHANNEL_1,Compare);
}

void TIM15_PWM_SetCompare2(uint16_t Compare)  
{
	__HAL_TIM_SetCompare(&htim15,TIM_CHANNEL_2,Compare);
}
    