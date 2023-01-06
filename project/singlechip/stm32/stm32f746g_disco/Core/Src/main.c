/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "crc.h"
#include "ltdc.h"
#include "usart.h"
#include "gpio.h"
#include "modular.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LOG(format, ...) usart_printf(&huart1, "LOG:" format "\r\n", ##__VA_ARGS__);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t img_data[sizeof(struct usart_data)];
struct usart_data *pimg;
uint8_t anchors[3][2] = {{9, 14}, {12, 17}, {22, 21}};
AI_ALIGNED(32)
static ai_i8 in_data[AI_NETWORK_IN_1_SIZE];
static ai_i8 out_data[AI_NETWORK_OUT_1_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int get_image() {

	memset(img_data, 0, sizeof(uint32_t));
	usart_read(&huart1, (uint8_t *)img_data, sizeof(uint32_t), USART_READ_TIMEOUT);
	
	if (pimg->st == USART_READ_START) {
	
		memset(img_data + sizeof(uint32_t), 0, IMAGE_SIZE + sizeof(uint32_t));
		usart_read(&huart1, (uint8_t *)img_data + sizeof(uint32_t), IMAGE_SIZE + sizeof(uint32_t), USART_READ_TIMEOUT);

		if (pimg->crc == USART_READ_CRC) {

			return 0;
		
		} else {
		
			LOG("The crc value is incorrect and the transmission data is abnormal this is not a complete data.");
			return -1;

		}

	}

	return -1;
}

void handle_data() {
	uint16_t *pdata = NULL;
	int ret = 0;
	int x1, y1, x2, y2;
	int grid_x, grid_y;
	float x, y, w ,h;

	pdata = (uint16_t *)pimg->data;
	
	for (int i = 0; i < IMAGE_HEIGHT - 1; ++i) {
	
		for (int j = 0; j < IMAGE_WIDTH - 1; ++j) {
			uint16_t color = pdata[j + (i * IMAGE_WIDTH)];

			in_data[(j + (i * IMAGE_WIDTH)) * AI_IM_C] = (int8_t)((color & IMAGE_COLOR_R) >> 9) - 128;
			in_data[((j + (i * IMAGE_WIDTH)) * AI_IM_C) + 1] = (int8_t)((color & IMAGE_COLOR_G) >> 3) - 128;
			in_data[((j + (i * IMAGE_WIDTH)) * AI_IM_C) + 2] = (int8_t)((color& IMAGE_COLOR_B) << 3) - 128;
		}
	}
	
	ret = ai_run(in_data, out_data);
	if (ret != 0) {
		
		LOG("Failed to run ai model: 0x%x\n", ret);
		return;
	}

	for (int i = 0; i < AI_OUTPUT_DIME; ++i) {
		
		for (int j = 0; j < AI_OUTPUT_LAYER; ++j) {
			
			int8_t conf = out_data[i * AI_OUTPUT_CAL + j * AI_OUTPUT_CAL_SIZE + AI_OUTPUT_CAL_CON];

			/* 70% */
			if(conf > AI_PER) {

				grid_x = i % 7;
				grid_y = (i - grid_x)/7;
				x = ((float)out_data[ i * AI_OUTPUT_CAL + j * AI_OUTPUT_CAL_SIZE] + AI_SCALE_INDEX) * AI_SCALE_OFFSET;
				y = ((float)out_data[ i * AI_OUTPUT_CAL + j * AI_OUTPUT_CAL_SIZE + 1] + AI_SCALE_INDEX) * AI_SCALE_OFFSET;
			  	w = ((float)out_data[ i * AI_OUTPUT_CAL + j * AI_OUTPUT_CAL_SIZE + 2] + AI_SCALE_INDEX) * AI_SCALE_OFFSET;
		  		h = ((float)out_data[ i * AI_OUTPUT_CAL + j * AI_OUTPUT_CAL_SIZE + 3] + AI_SCALE_INDEX) * AI_SCALE_OFFSET;
				
				x = (algo_sigmoid(x) + grid_x) * AI_SAMPLIN;
				y = (algo_sigmoid(y) + grid_y) * AI_SAMPLIN;
				w = expf(w) * anchors[j][0];
				h = expf(h) * anchors[j][1];
				y1 = (y - h / 2);
				y2 = (y + h / 2);
				x1 = (x - w / 2);
				x2 = (x + w / 2);

				if(x1 < 0) x1 = 0;
				if(y1 < 0) y1 = 0;
				if(x2 > 55) x2 = 55;
				if(y2 > 55) y2 = 55;
				
				//x2 = x2 - (x2 / 4);
				//y2 = y2 - (y2 / 4);
				LOG("%d,%d,%d,%d", x1, y1, x2 - (x2 / 4), y2 - (y2 / 4));
				draw_rect(pimg->data, x1, y1, x2, y2, IMAGE_WIDTH, IMAGE_HEIGHT, 2, 0x00f8, 4);
			}
		}
	}
			
}
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

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_CRC_Init();
  MX_LTDC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  if (ai_init() != 0) {

	  LOG("Failed to initialize ai model");
	  return -1;
  }
  pimg = (struct usart_data *)img_data;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
  	if (get_image() == 0) {

		handle_data();
	}
    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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
