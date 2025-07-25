/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Servo.h"
#include "Pump.h"
#include <stdio.h>
#include "PID.h"
#include "DHT11.h"
#include "MQTT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

	uint16_t current_compare_1D=1500;
	uint16_t current_compare_2D=1500;
	uint8_t rxByte;
	uint8_t Direction;
	
	uint8_t rx6;
  uint8_t frame6[9]={0};
  uint8_t frame6_idx = 0;
	
	uint8_t Pump;
	
	uint8_t rx3[5];
	uint8_t tag=0;
	uint8_t tag_1=0;
	uint8_t tag_2=0;
	uint8_t count=0;
	uint16_t count_1=0;
	
  static uint8_t flame_x = 60, flame_y = 84;
	
	extern uint8_t Data[5];
	
	char x[2]="x";
  char password[9]="12121212";
  char client_id[33]="43efe7b7661db39ee9d4b73038261ed4";
  char a[1];
  char topic1[12]= "Temperature";
  char Temperature[3]; 
  char topic2[9]= "Humidity";
  char Humidity[3]; 
	
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM15_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	Servo_Start();
	
	MQTT_Init(x, password, client_id);
	
	HAL_UART_Receive_IT(&huart2, &rxByte, 1);
	
	HAL_UART_Receive_IT(&huart6, &rx6, 1);
	
  HAL_UART_Receive_IT(&huart3, rx3, 4);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		


		if(frame6[7]==0x33)
		{
			current_compare_1D=1500;
			current_compare_2D=1500;
			Servo_Compare(current_compare_1D,current_compare_2D);
		}
		else if(frame6[7]==0x22)
		{
			if(Direction==0x01){
			  Servo_turn_left(&current_compare_1D);
			  HAL_Delay(100);
		  }
		  else if(Direction==0x02){
			  Servo_turn_right(&current_compare_1D);
		  	HAL_Delay(100);
		  }
		  else if(Direction==0x03){
			  Servo_turn_up(&current_compare_2D);
			  HAL_Delay(100);
		  }
		  else if(Direction==0x04){
			  Servo_turn_down(&current_compare_2D);
			  HAL_Delay(100);
		  }
		
		  if(Pump==0x06){
			  Pump_ON();
		  }
		  else if(Pump==0x07){
			  Pump_Off();
		  }
		}
		else if(frame6[7]==0x11)
			{
				if(tag==1)
				{
					PID_1D(flame_x,&current_compare_1D);
					PID_2D(flame_y,&current_compare_2D);
					tag=0;
					HAL_Delay(30);
				}
				else if(tag==0)
				{
					count++;
					if(count>15)
					{
						Pump_Off();
						count=0;
					}

					if(current_compare_1D>=1860)
					{
						tag_1=0;
					}
					else if(current_compare_1D<=1140)
					{
						tag_1=1;
					}
					
					if(tag_1==0)
					{
						Servo_turn_right(&current_compare_1D);
		  	    HAL_Delay(60);
					}
					else if(tag_1==1)
					{
						Servo_turn_left(&current_compare_1D);
			      HAL_Delay(60);
					}
					
					
					if(current_compare_2D>=1530)
					{
						tag_2=0;
					}
					else if(current_compare_2D<=1270)
					{
						tag_2=1;
					}
					
					if(tag_2==0)
					{
						Servo_turn_down(&current_compare_2D);
		  	    HAL_Delay(50);
					}
					else if(tag_2==1)
					{
						Servo_turn_up(&current_compare_2D);
			      HAL_Delay(50);
					}

				}
			}
		else if(frame6[7]==0x88)
		{
			
			if(DHT_Read())
			{
				uint8_t humidity = Data[0];
				uint8_t temperature = Data[2];

				char cmd[50];
				snprintf(cmd, sizeof(cmd), "page2.n1.val=%d%c%c%c", Data[2], 0xFF, 0xFF, 0xFF);
				HAL_UART_Transmit(&huart6, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);
				
				snprintf(cmd, sizeof(cmd), "page2.n0.val=%d%c%c%c", Data[0], 0xFF, 0xFF, 0xFF);
				HAL_UART_Transmit(&huart6, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);
				
				if(Data[2]>65)
				{
					count_1++;
					if(count_1>=6)
					{
						HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
						count_1=8;
					}
					else
					{
						HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);	
					}				
				}
				else
				{
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
		      count_1=0;
				}
				
				uint32_t now = HAL_GetTick();
				
				snprintf(Temperature, sizeof(Temperature), "%d", Data[2]);
        MQTT_update(topic1, Temperature);
				
				snprintf(Humidity, sizeof(Humidity), "%d", Data[0]);
        MQTT_update(topic2, Humidity);
			}
		}
					
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 220;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if (rxByte == 0x01) {
          Direction=rxByte;
					HAL_UART_Receive_IT(&huart2, &rxByte, 1);
				}
				else if (rxByte == 0x02) {
          Direction=rxByte;
					HAL_UART_Receive_IT(&huart2, &rxByte, 1);
				}
				else if (rxByte == 0x03) {
          Direction=rxByte;
					HAL_UART_Receive_IT(&huart2, &rxByte, 1);
				}
				else if (rxByte == 0x04) {
          Direction=rxByte;
					HAL_UART_Receive_IT(&huart2, &rxByte, 1);
				}
				else if (rxByte == 0x00) {
          Direction=rxByte;
					HAL_UART_Receive_IT(&huart2, &rxByte, 1);
				}
				else if (rxByte == 0x06) {
          Pump=rxByte;
					HAL_UART_Receive_IT(&huart2, &rxByte, 1);
				}
				else if (rxByte == 0x07) {
          Pump=rxByte;
					HAL_UART_Receive_IT(&huart2, &rxByte, 1);
				}			
    }
		
		 else if (huart->Instance == USART6)
     {
			frame6[frame6_idx++] = rx6;
      if (frame6_idx == 9){ 
        frame6_idx = 0;
      }
      HAL_UART_Receive_IT(&huart6, &rx6, 1);
		 }
		 
		 else if (huart->Instance == USART3)
    { 
      if(rx3[0]==0x3c)
			{
				if(rx3 [1]==0x3b)
				{			
            flame_x=rx3[2];
            flame_y=rx3[3];	
						tag=1;
				}
			}  	 	
       HAL_UART_Receive_IT(&huart3, rx3, 4);
    }
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
