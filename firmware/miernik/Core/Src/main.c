/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>
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
  uint8_t i2c_OLED_connected = 0;
  uint8_t i2c_volt_sensor_connected = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


//read data from INA detector
#define INA226_ADDR (0x40 << 1)
#define INA226_REG_BUS_VOLT 0x02
#define INA226_REG_CONFIG 0x00
#define INA226_REG_POWER 0x03
#define INA226_REG_CURRENT 0x04
#define INA226_REG_CALIB 0x05


#define INA226_CURRENT_LSB  0.0001f
#define INA226_POWER_LSB    (25.0f * INA226_CURRENT_LSB)


HAL_StatusTypeDef INA226_WriteRegister(uint8_t reg, uint16_t value)
{
  uint8_t buf[2];
  buf[0] = (uint8_t)(value >> 8);
  buf[1] = (uint8_t)(value & 0xFF);
  return HAL_I2C_Mem_Write(&hi2c2, INA226_ADDR, reg, 1, buf, 2, 20);
}


float INA226_ReadBusVoltage()
{
  uint8_t buf[2];
  if (HAL_I2C_Mem_Read(&hi2c2,INA226_ADDR, INA226_REG_BUS_VOLT, 1, buf, 2, 20)==HAL_OK)
  {
    uint16_t raw = (buf[0] << 8) | buf[1];
    return (float)raw * 0.00125f;
  }
  return 0.0f;
}

float INA226_ReadCurrent()
{
  uint8_t buf[2];
  if (HAL_I2C_Mem_Read(&hi2c2,INA226_ADDR, INA226_REG_CURRENT, 1, buf, 2, 20)==HAL_OK)
  {
    int16_t raw = (int16_t)((buf[0] << 8) | buf[1]);
    return (float)raw * INA226_CURRENT_LSB;
  }
  return 0.0f;
}

float INA226_ReadPower()
{
  uint8_t buf[2];
  if (HAL_I2C_Mem_Read(&hi2c2,INA226_ADDR, INA226_REG_POWER, 1, buf, 2, 20)==HAL_OK)
  {
    uint16_t raw = (uint16_t)((buf[0] << 8) | buf[1]);
    return (float)raw * INA226_POWER_LSB;
  }
  return 0.0f;
}




void LED1_Update(uint8_t is_connected){
  static uint32_t last_toggle = 0;
  uint32_t now = HAL_GetTick();
  if (is_connected){
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
  } else{
    if (now - last_toggle >= 100){
      last_toggle = now;
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    }
  }
}

void LED2_Update(uint8_t is_connected){
  static uint32_t last_toggle = 0;
  uint32_t now = HAL_GetTick();
  if (is_connected){
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
  } else{
    if (now - last_toggle >= 100){
      last_toggle = now;
      HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    }
  }
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
  MX_USART2_UART_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();

  ssd1306_Fill(Black);
  ssd1306_SetCursor(10,20);
  ssd1306_WriteString("Hello!", Font_11x18, White);
  ssd1306_UpdateScreen();

  HAL_Delay(2000);

  // inicialization of INA226
  INA226_WriteRegister(INA226_REG_CALIB, 5120); // value vor 0.01 Ohm

  INA226_WriteRegister(INA226_REG_CONFIG, 0x4727);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
while (1)
  {
    LED1_Update(i2c_OLED_connected);
    LED2_Update(i2c_volt_sensor_connected);

    static uint32_t last_i2c_check = 0;
    if (HAL_GetTick() - last_i2c_check >= 500)
    {
      last_i2c_check = HAL_GetTick();

      if (HAL_I2C_IsDeviceReady(&hi2c2, (0x3C << 1), 2, 10) == HAL_OK)
      {
        i2c_OLED_connected = 1;
      }
      else
      {
        i2c_OLED_connected = 0;
      }
      if (HAL_I2C_IsDeviceReady(&hi2c2, (0x40 << 1), 2, 10) == HAL_OK)
      {
        i2c_volt_sensor_connected = 1;
      }
      else
      {
        i2c_volt_sensor_connected = 0;
      }
      
    }

    static uint32_t last_OLED_refresh = 0;
    if (HAL_GetTick() - last_OLED_refresh >= 200)
    {
      last_OLED_refresh = HAL_GetTick();
      if (i2c_volt_sensor_connected && i2c_OLED_connected)
      {
        float voltage = INA226_ReadBusVoltage();
        float current = INA226_ReadCurrent();
        float power = INA226_ReadPower();


        char display_voltage[16];
        uint16_t v_int = (uint16_t)voltage;
        uint16_t v_dec = (uint16_t)((voltage - v_int) * 100);
        snprintf(display_voltage, sizeof(display_voltage), "%d.%02d V", v_int, v_dec);


        char display_current[16];
        uint16_t curr_int = (uint16_t)current;
        uint16_t curr_dec = (uint16_t)((current - curr_int) * 100);
        snprintf(display_current, sizeof(display_current), "%d.%02d A", curr_int, curr_dec);

        char display_power[16];
        uint16_t power_int = (uint16_t)power;
        uint16_t power_dec = (uint16_t)((power - power_int) * 100);
        snprintf(display_power, sizeof(display_power), "%d.%02d W", power_int, power_dec);

        //display
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 5);
        ssd1306_WriteString("Voltage:", Font_7x10, White);
        ssd1306_SetCursor(60, 5);
        ssd1306_WriteString(display_voltage, Font_7x10, White);

        ssd1306_SetCursor(0, 20);
        ssd1306_WriteString("Current:", Font_7x10, White);
        ssd1306_SetCursor(60, 20);
        ssd1306_WriteString(display_current, Font_7x10, White);

        ssd1306_SetCursor(0, 35);
        ssd1306_WriteString("Power:", Font_7x10, White);
        ssd1306_SetCursor(60, 35);
        ssd1306_WriteString(display_power, Font_7x10, White);


        ssd1306_UpdateScreen();
      }
    }
    __WFI();
    /* USER CODE END WHILE */


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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
