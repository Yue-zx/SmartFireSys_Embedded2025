#include "stm32h7xx.h"                  // Device header
#include "PID.h"
#include "PWM.h"
#include "Pump.h"

void PID_1D(uint8_t flame_x,uint16_t *current_compare_1D)
{
	const int16_t target_x = 64;
	int16_t err1 = target_x - (int16_t)flame_x;
	static int16_t last_err1 = 0;
	float PID_Out1=0;
	
	const float Kp = 1;
	const float Kd = 0.3;
	
	int16_t derivative = err1 - last_err1; 
	
  PID_Out1 = Kp * err1 + Kd * derivative;
	
	
//	if(err1<=2)
//	{
//		Pump_ON();
//	}
//	else if(err1>3)
//	{
//		void Pump_Off();
//	}
	
	*current_compare_1D=*current_compare_1D+(int16_t)PID_Out1;
	
	last_err1 = err1;
	
	if(*current_compare_1D<=840)
	{
		*current_compare_1D=840;
	}
	else if(*current_compare_1D>=2160)
	{
		*current_compare_1D=2160;
	}

	TIM15_PWM_SetCompare1(*current_compare_1D);
}

void PID_2D(uint8_t flame_y,uint16_t *current_compare_2D)
{
	const int16_t target_y = 70;
	int16_t err2 = target_y - (int16_t)flame_y;
	static int16_t last_err2 = 0;
	float PID_Out2=0;
	
	
	const float Kp = 1;
	const float Kd = 0.3;
	
	int16_t derivative = err2 - last_err2;
	
  PID_Out2 = Kp * err2 + Kd * derivative;
	
	
	*current_compare_2D=*current_compare_2D+(int16_t)PID_Out2;
	
	last_err2 = err2;
	
	if(*current_compare_2D<=1170)
	{
		*current_compare_2D=1170;
	}
	else if(*current_compare_2D>=1830)
	{
		*current_compare_2D=1830;
	}

	TIM15_PWM_SetCompare2(*current_compare_2D);
}


/*
float pid_calc(PID_STRUCT *data, float fdb_val) //参数和目标值
{
    
    data->fdb = fdb_val;  // 更新反馈值
    data->error = data->set - data->fdb;  //当前误差
    data->Pout = data->error;   // 比例项
    data->integral += data->error;     //积分项
	
    // 积分限幅
    if(data->integral > data->i_max) 
		{
			data->integral = data->i_max;
    }
		else if(data->integral < -data->i_max) 
			{
				data->integral = -data->i_max;
			}
    data->Iout = data->integral;
    
    // 微分项
    data->Dout = data->error - data->error_last;
    
    //输出
    data->out = data->Kp * data->Pout 
              + data->Ki * data->Iout 
              + data->Kd * data->Dout;
    
    // 更新历史误差
    data->error_last = data->error; 
    return data->out;
}

PID_STRUCT motor_pid;
PID_STRUCT temp_pid;

*/

/*
int main(void) {
    // 初始化PID参数
    PID_Init(&motor_pid, 0.8, 0.05, 0.1);
    PID_Init(&temp_pid, 2.0, 0.01, 0.0);
    
    // 设置目标值
    motor_pid.set = 1000;   // 电机目标转速
    temp_pid.set = 50;      // 温度目标值
    
    while(1) {
        // 电机控制
        float motor_fdb = read_motor_speed();
        float motor_out = pid_calc(&motor_pid, motor_fdb);
        set_motor_power(motor_out);
        
        // 温度控制
        float temp_fdb = read_temperature();
        float heater_out = pid_calc(&temp_pid, temp_fdb);
        set_heater_power(heater_out);
        
        delay_ms(10);
    }
}

*/