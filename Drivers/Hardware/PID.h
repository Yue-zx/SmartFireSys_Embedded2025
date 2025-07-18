#ifndef __PID_H__
#define __PID_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void PID_1D(uint8_t flame_x,uint16_t *current_compare_1D);
void PID_2D(uint8_t flame_y,uint16_t *current_compare_2D);

#ifdef __cplusplus
}
#endif
#endif



/*
typedef struct {
    float set;       // 设定值
    float fdb;       // 反馈值
    float Kp, Ki, Kd;
    
    // 误差
    float error;      // 当前误差
    float error_last; // 上次误差
    
    // 项输出
    float Pout;
    float Iout;
    float Dout;
    float out;
    
    // 新增积分累积
    float integral;   // 积分累积量
    float i_max;      // 积分限幅
} PID_STRUCT;

float pid_calc(PID_STRUCT *data, float fdb_val);
*/

