#include "PWM.h"

void Servo_Start()
{
	TIM15_PWM_Start();
}

void Servo_Compare(uint16_t current_compare_1D,uint16_t current_compare_2D)
{
	TIM15_PWM_SetCompare1(current_compare_1D);
	TIM15_PWM_SetCompare2(current_compare_2D);
}

void Servo_SetAngle(float Angle1[2])
{
	TIM15_PWM_SetCompare1(Angle1[0]/270*2000+500);
	TIM15_PWM_SetCompare2(Angle1[1]/180*2000+500);
}

void Servo_turn_right(uint16_t *current_compare_1D)
{
	if(*current_compare_1D<=840)
	{
		*current_compare_1D=*current_compare_1D;
	}
	else
	{
		*current_compare_1D=*current_compare_1D-5;
	}
	TIM15_PWM_SetCompare1(*current_compare_1D);
}

void Servo_turn_left(uint16_t *current_compare_1D)
{
	if(*current_compare_1D>=2160)
	{
		*current_compare_1D=*current_compare_1D;
	}
	else
	{
		*current_compare_1D=*current_compare_1D+5;
	}
	TIM15_PWM_SetCompare1(*current_compare_1D);
}
	
void Servo_turn_up(uint16_t *current_compare_2D)
{
	if(*current_compare_2D>=1830)
	{
		*current_compare_2D=*current_compare_2D;
	}
	else
	{
		*current_compare_2D=*current_compare_2D+5;
	}
	TIM15_PWM_SetCompare2(*current_compare_2D);
}

void Servo_turn_down(uint16_t *current_compare_2D)
{
		if(*current_compare_2D<=1170)
	{
		*current_compare_2D=*current_compare_2D;
	}
	else
	{
		*current_compare_2D=*current_compare_2D-5;
	}
	TIM15_PWM_SetCompare2(*current_compare_2D);
}
