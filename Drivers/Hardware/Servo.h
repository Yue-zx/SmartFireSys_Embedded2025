#ifndef __SERVO_H__
#define __SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void Servo_Start();
void Servo_Compare(uint16_t current_compare_1D,uint16_t current_compare_2D);
void Servo_SetAngle(float Angle1[2]);
void Servo_turn_right(uint16_t *current_compare_1D);
void Servo_turn_left(uint16_t *current_compare_1D);
void Servo_turn_up(uint16_t *current_compare_2D);
void Servo_turn_down(uint16_t *current_compare_2D);
	
#ifdef __cplusplus
}
#endif
#endif
