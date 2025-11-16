/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "string.h"
int count2 = -1;
int rise = 1;
int active = 0;
int count = -1;
int t1;
int t2;
int rise_time;
int bit_ready = 0;
int bits[8];
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */


  char a = '0';
//  int count2 = -1;
  char receive_char(){

	  HAL_UART_Transmit(&huart2, "Signal of the character: ", strlen("Signal of the character: "), HAL_MAX_DELAY);

	  int num = 0;
	  while(1){

		  if(count==count2+1){
			  rise_time = (HAL_GetTick()-t1)-50;
//			  num = count;
//			  sprintf(str, "%d", num);
//			  HAL_UART_Transmit(&huart2, str, strlen(str), HAL_MAX_DELAY);
//			  HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
			  t1 = HAL_GetTick();
			  count2++;
		  if(count>0){

//			  num = rise_time;
//			  sprintf(str, "%d", num);
//			  HAL_UART_Transmit(&huart2, str, strlen(str), HAL_MAX_DELAY);
//			  HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
			  if(rise_time>(150-20) && rise_time<(150+20)){
				  HAL_UART_Transmit(&huart2, "1", strlen("1"), HAL_MAX_DELAY);
//				  HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
				  bits[count-1] = 1;
			  }
			  else if(rise_time>(50-20) && rise_time<(50+20)){
				  HAL_UART_Transmit(&huart2, "0", strlen("1"), HAL_MAX_DELAY);
//				  HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
				  bits[count-1] = 0;
			  }
			  if(count == 8){
				  HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
//				  HAL_UART_Transmit(&huart2, "hello", strlen("hello"), HAL_MAX_DELAY);
//				  HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY);
//				  count = -1;
				  num = 0;
				  for (int i = 0; i < 8;i++){
					  num = (num*2) + bits[i];
				  }
				  a = num;


				  active = 0;
				  bit_ready = 0;
				  return a;


			  }
			  bit_ready = 0;
		  }
		  }
	  }
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if(active == 1){
		  a = receive_char();
		  HAL_UART_Transmit(&huart2, "Your character was: ", strlen("Your character was: "), HAL_MAX_DELAY);
		  HAL_UART_Transmit(&huart2, &a, 1, HAL_MAX_DELAY);
		  HAL_UART_Transmit(&huart2, "\r\n\r\n", 4, HAL_MAX_DELAY);
	  }
//	  HAL_UART_Receive(&huart2, &a, 1, HAL_MAX_DELAY);
//	  HAL_UART_Transmit(&huart2, &a, 1, HAL_MAX_DELAY);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

    if(GPIO_Pin == GPIO_PIN_0){

//    	}

			if(rise==1){
			rise = 0;
	//		t1 = HAL_GetTick();
			}
			else{
			rise = 1;
	//		t2 = HAL_GetTick();
			}

			if(count ==-1){
	//    		t1 = HAL_GetTick();
				active =1;
				count++;
			}
			else if(count == 8 && rise==1){
			//    		bit_ready = 0;
					  rise = 1;
		//			  active = 0;
		//			  rise = 1;
					//  active = 0;
					  count = -1;
					  count2 = -1;
					  //bit_ready = 0;
					  active = 0;
			}
			else{
				if(rise==1){
				}
				else{
	//    			rise_time = t2 - t1;
	//    			t1 = HAL_GetTick();
					bit_ready = 1;
					count++;
				}
			}
		}

}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : data_in_Pin */
  GPIO_InitStruct.Pin = data_in_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(data_in_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
