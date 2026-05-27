/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "ssd1306.h"
#include "fonts.h"
#include <stdio.h>
#include <string.h>
#include "oled.h"
#include "write-letters.h"
#include "square2.h"
#include "bluetooth.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define RX_BUFFER_SIZE        32
#define OLED_CHANGE_THRESHOLD 2    /* Atualiza OLED a cada N mudanças de posição */
#define OLED_MIN_INTERVAL_MS  50    /* Intervalo mínimo entre updates do OLED (ms) */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;

I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart5;

/* USER CODE BEGIN PV */

uint8_t rx_byte;
char rx_buffer[32];
uint8_t rx_index = 0;
volatile uint8_t duty_r = 0, duty_g = 0, duty_b = 0;
volatile uint8_t update_display = 1;
uint32_t adc_val_x = 0, adc_val_y = 0;
uint8_t pos_x = 0, pos_y = 0;
char msg_buffer[100];

/* Controle de atualização do OLED por contagem de mudanças */
static uint8_t  pos_x_prev      = 0xFF;  /* Valor impossível para forçar 1ª atualização */
static uint8_t  pos_y_prev      = 0xFF;
static uint8_t  change_count    = 0;
static uint32_t last_oled_ms    = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C2_Init(void);
static void MX_UART5_Init(void);
static void MX_ADC3_Init(void);
static void MX_LPUART1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Callback de recepção da UART */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == UART5)
  {
    if (rx_byte == '\n' || rx_byte == '\r')
    {
      rx_buffer[rx_index] = '\0';
      bluetoothCommand(rx_buffer, &duty_r, &duty_g, &duty_b, &update_display);
      rx_index = 0;
    }
    else
    {
      if (rx_byte != '%')
        rx_buffer[rx_index++] = rx_byte;
      if (rx_index >= RX_BUFFER_SIZE - 1)
        rx_index = 0;
    }
    HAL_UART_Receive_IT(&huart5, &rx_byte, 1);
  }
}

/* Interrupção externa PA11 — botão de reset de cor */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_11)
  {
    duty_r = 0;
    duty_g = 0;
    duty_b = 0;

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);

    update_display = 1;
  }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* MCU Configuration -------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_I2C2_Init();
  MX_UART5_Init();
  MX_ADC3_Init();
  MX_LPUART1_Init();

  /* USER CODE BEGIN 2 */
  SSD1306_Init();
  SSD1306_Clear();
  SSD1306_UpdateScreen();

  /* Inicia os canais PWM */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); /* RED   */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); /* GREEN */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4); /* BLUE  */

  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty_r);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty_g);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, duty_b);

  /* Inicia recepção UART por interrupção */
  HAL_UART_Receive_IT(&huart5, &rx_byte, 1);

  /* Calibra ADC3 */
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  ADC_ChannelConfTypeDef sConfig = {0};

  while (1)
  {
    /* ------------------------------------------------------------------ */
    /* 1. LEITURA ADC — EIXO X (Canal 1)                                  */
    /* ------------------------------------------------------------------ */
    sConfig.Channel      = ADC_CHANNEL_1;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff   = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset       = 0;
    HAL_ADC_ConfigChannel(&hadc3, &sConfig);

    HAL_ADC_Start(&hadc3);
    HAL_ADC_PollForConversion(&hadc3, 10);
    adc_val_x = HAL_ADC_GetValue(&hadc3);
    HAL_ADC_Stop(&hadc3);

    /* ------------------------------------------------------------------ */
    /* 2. LEITURA ADC — EIXO Y (Canal 12)                                 */
    /* ------------------------------------------------------------------ */
    sConfig.Channel = ADC_CHANNEL_12;
    HAL_ADC_ConfigChannel(&hadc3, &sConfig);

    HAL_ADC_Start(&hadc3);
    HAL_ADC_PollForConversion(&hadc3, 10);
    adc_val_y = HAL_ADC_GetValue(&hadc3);
    HAL_ADC_Stop(&hadc3);

    /* ------------------------------------------------------------------ */
    /* 3. MAPEAMENTO COM ARREDONDAMENTO E INVERSÃO                        */
    /* ------------------------------------------------------------------ */
    pos_x = (uint8_t)(((adc_val_x * 6U) + 2047U) / 4095U);
    pos_y = (uint8_t)(6U - (((adc_val_y * 6U) + 2047U) / 4095U));

    if (pos_x > 6) pos_x = 6;
    if (pos_y > 6) pos_y = 6;

    /* ------------------------------------------------------------------ */
    /* 4. ATUALIZAR MATRIZ DE LEDs — PRIORIDADE MÁXIMA, SEM PISCAR        */
    /*    Roda a cada iteração do loop, sem nenhuma condição de tempo.     */
    /* ------------------------------------------------------------------ */
    Draw_Square_2x2(pos_x, pos_y);

    /* ------------------------------------------------------------------ */
    /* 5. ATUALIZAR OLED — só quando houver N mudanças de posição         */
    /*    E respeitando um intervalo mínimo para não sobrecarregar o I2C. */
    /* ------------------------------------------------------------------ */

    /* Detecta mudança real de posição */
    if (pos_x != pos_x_prev || pos_y != pos_y_prev)
    {
      pos_x_prev = pos_x;
      pos_y_prev = pos_y;
      change_count++;
    }

    /* Atualiza OLED se atingiu o threshold de mudanças
       E se já passou o intervalo mínimo desde o último update */
    if (change_count >= OLED_CHANGE_THRESHOLD &&
        (HAL_GetTick() - last_oled_ms) >= OLED_MIN_INTERVAL_MS)
    {
      last_oled_ms  = HAL_GetTick();
      change_count  = 0;

      SSD1306_Clear();

      sprintf(msg_buffer, "ADC X: %lu", adc_val_x);
      SSD1306_GotoXY(5, 5);
      SSD1306_Puts(msg_buffer, &Font_7x10, SSD1306_COLOR_WHITE);

      sprintf(msg_buffer, "ADC Y: %lu", adc_val_y);
      SSD1306_GotoXY(5, 20);
      SSD1306_Puts(msg_buffer, &Font_7x10, SSD1306_COLOR_WHITE);

      sprintf(msg_buffer, "Pos: (%d,%d)", pos_x, pos_y);
      SSD1306_GotoXY(5, 35);
      SSD1306_Puts(msg_buffer, &Font_7x10, SSD1306_COLOR_WHITE);

      SSD1306_UpdateScreen();
    }

    /* USER CODE END WHILE */
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

  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM           = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN           = 85;
  RCC_OscInitStruct.PLL.PLLP           = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ           = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR           = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                     RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief ADC3 Initialization Function
 * @retval None
 */
static void MX_ADC3_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc3.Instance                   = ADC3;
  hadc3.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution            = ADC_RESOLUTION_12B;
  hadc3.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc3.Init.GainCompensation      = 0;
  hadc3.Init.ScanConvMode          = ADC_SCAN_DISABLE;
  hadc3.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait      = DISABLE;
  hadc3.Init.ContinuousConvMode    = DISABLE;
  hadc3.Init.NbrOfConversion       = 1;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.Overrun               = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.OversamplingMode      = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel      = ADC_CHANNEL_1;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff   = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset       = 0;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief I2C2 Initialization Function
 * @retval None
 */
static void MX_I2C2_Init(void)
{
  hi2c2.Instance              = I2C2;
  hi2c2.Init.Timing           = 0x40621236;
  hi2c2.Init.OwnAddress1      = 0;
  hi2c2.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2      = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief TIM2 Initialization Function
 * @retval None
 */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig     = {0};
  TIM_OC_InitTypeDef sConfigOC             = {0};

  htim2.Instance               = TIM2;
  htim2.Init.Prescaler         = 1699;
  htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim2.Init.Period            = 100;
  htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode     = TIM_OCMODE_PWM1;
  sConfigOC.Pulse      = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.Pulse = 499;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim2);
}

/**
 * @brief UART5 Initialization Function
 * @retval None
 */
static void MX_UART5_Init(void)
{
  huart5.Instance            = UART5;
  huart5.Init.BaudRate       = 9600;
  huart5.Init.WordLength     = UART_WORDLENGTH_8B;
  huart5.Init.StopBits       = UART_STOPBITS_1;
  huart5.Init.Parity         = UART_PARITY_NONE;
  huart5.Init.Mode           = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl      = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling   = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief LPUART1 Initialization Function
 * @retval None
 */
static void MX_LPUART1_Init(void)
{
  hlpuart1.Instance            = LPUART1;
  hlpuart1.Init.BaudRate       = 115200;
  hlpuart1.Init.WordLength     = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits       = UART_STOPBITS_1;
  hlpuart1.Init.Parity         = UART_PARITY_NONE;
  hlpuart1.Init.Mode           = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl      = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief GPIO Initialization Function
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOC,
    GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
    GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
    GPIO_PIN_RESET);

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB,
    GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
    GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9,
    GPIO_PIN_RESET);

  /* Botão B1 — interrupção na borda de subida */
  GPIO_InitStruct.Pin  = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /* GPIOC — saídas da matriz */
  GPIO_InitStruct.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* LPUART1 TX/RX */
  GPIO_InitStruct.Pin       = LPUART1_TX_Pin | LPUART1_RX_Pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF12_LPUART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* LED LD2 */
  GPIO_InitStruct.Pin   = LD2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* GPIOB — saídas da matriz */
  GPIO_InitStruct.Pin   = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                          GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* PA11 — botão de reset de cor (borda de descida, pull-up interno) */
  GPIO_InitStruct.Pin  = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
 * @brief  Error Handler
 * @retval None
 */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
