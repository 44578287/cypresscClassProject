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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 重定向输出
#define REDIRECT_OUTPUT
#include "rom.h"
#include "settings.h"
#include "gpioPlus.h"
#include "event.h"

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
AT24C08C eeprom(&hi2c1, 0x50); // EEPROM

#define RX_BUFFER_SIZE 256                 // 环形缓冲区大小
volatile uint8_t rxBuffer[RX_BUFFER_SIZE]; // 环形缓冲区
volatile uint16_t rxHead = 0;              // 写指针
volatile uint16_t rxTail = 0;              // 读指针

volatile bool dataReceivedFlag = false; // 数据接收标志

Gpio led5(GPIOA, GPIO_PIN_6), // PA6
    led6(GPIOA, GPIO_PIN_7);  // PA7

volatile uint8_t mode = 0;
uint8_t lastMode = 0;

extern "C"
{
  /// @brief 100ms回调
  extern Event<> milliseconds_100;
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void StartUARTReceive();
void ProcessReceivedData(uint8_t data);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ledclear()
{
  led5.off();
  led6.off();
}

void ledModeSwitch(uint8_t mode)
{
  ledclear();
  // printf("模式切换: %d\n", mode);
  switch (mode)
  {
  case 1:
    led5.on();
    milliseconds_100.registerCallback([]()
                                      {
      led5.toggle();
      led6.toggle(); });
    break;
  case 2:
    milliseconds_100.registerCallback([]()
                                      {
      led5.toggle();
      led6.toggle(); });
    break;

  default:
    printf("模式不存在\n");
    ledclear();
    return;
    break;
  }
  printf("Mode%d activated\n", mode);
}
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  StartUARTReceive();
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  ledclear();
  printf("初始化完成\n");
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // 串口接收数据处理
    if (dataReceivedFlag)
    {
      dataReceivedFlag = false;
      // 处理接收到的数据
      while (rxTail != rxHead)
      {
        uint8_t data = rxBuffer[rxTail];
        rxTail = (rxTail + 1) % RX_BUFFER_SIZE;
        ProcessReceivedData(data);
      }
    }

    //LED模式切换
    if (lastMode != mode)
    {
      ledModeSwitch(mode);
      lastMode = mode;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USER CODE BEGIN 4 */
/// @brief 处理接收到的数据
void ProcessReceivedData(uint8_t data)
{
  static uint8_t tempSerialData[RX_BUFFER_SIZE];
  static uint16_t tempSerialDataIndex = 0;

  tempSerialData[tempSerialDataIndex] = data;
  tempSerialDataIndex++;
  if (data == 0x21) // '!' 表示结束符
  {
    tempSerialData[tempSerialDataIndex - 1] = 0;
    tempSerialDataIndex = 0;
    printf("输入命令: %s\n", tempSerialData);
    char *p;
    if ((p = strstr((char *)tempSerialData, "mode")) != NULL)
    {
      printf("参数: %c\n", p[4]);
      mode = p[4] - 48;
    }
    else if ((p = strstr((char *)tempSerialData, "w 0xb")) != NULL)
    {
      unsigned int addr = 0, data = 0;

      if (sscanf(p + 5, "%2x%2x", &addr, &data) == 2)
      {
        printf("写入数据: 地址0x%x 数据0x%x\n", addr, data);
        if (eeprom.writeByte(addr, data) != HAL_ERROR)
          printf("OK\n");
        else
          printf("FAIL\n");
      }
      else
        printf("解析失败: 输入格式不正确\n");
    }
    else if ((p = strstr((char *)tempSerialData, "r 0xb")) != NULL)
    {
      unsigned int addr = 0;
      uint8_t data;

      if (sscanf(p + 5, "%2x", &addr) == 1)
      {
        if (eeprom.readByte(addr, data) != HAL_ERROR)
        {
          printf("读取数据: 地址0x%x 数据0x%x\n", addr, data);
          printf("OK\n");
        }
        else
          printf("FAIL\n");
      }
      else
        printf("解析失败: 输入格式不正确\n");
    }
    else
    {
      printf("未知命令\n");
    }
  }

  if (tempSerialDataIndex >= RX_BUFFER_SIZE)
  {
    tempSerialDataIndex = 0;
  }
}

void StartUARTReceive(void)
{
  if (HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxBuffer[rxHead], 1) != HAL_OK)
  {
    Error_Handler();
  }
}

/// @brief 串口接收中断回调函数
/// @param huart
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    rxHead = (rxHead + 1) % RX_BUFFER_SIZE;

    // 缓冲区溢出
    if (rxHead == rxTail)
    {
      rxTail = (rxTail + 1) % RX_BUFFER_SIZE; // 覆盖数据
    }

    dataReceivedFlag = true;

    // 继续接收下一个字节
    HAL_UART_Receive_IT(huart, (uint8_t *)&rxBuffer[rxHead], 1);
  }
}

/// @brief IO中断回调函数
/// @param GPIO_Pin
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_8)
  {
    mode = 2;
  }
  if (GPIO_Pin == GPIO_PIN_15)
  {
    mode = 1;
  }
}
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

#ifdef USE_FULL_ASSERT
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
