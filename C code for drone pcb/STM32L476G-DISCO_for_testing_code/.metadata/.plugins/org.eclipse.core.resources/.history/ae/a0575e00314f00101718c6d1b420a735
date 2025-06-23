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
#include "nrf24.h"
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
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void nRF24_set_up(uint8_t width, uint8_t channel, uint8_t data_rate, uint8_t power_level, uint8_t CRC_mode, uint8_t delay, uint8_t attempts, uint8_t rx_pipe, uint8_t *addr, uint8_t AA_state, uint8_t payload_size);
void nRF24_start_sending(uint8_t *data, uint8_t length);
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
 // uint8_t send_spi_reg_addr = {0x8f}; // 10 001111
 // uint8_t rec_spi_id_data= {0x00};
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  uint8_t addr[5] = {'D', 'R', 'O', 'N', 'E'};
  nRF24_set_up(5, 78, nRF24_DR_2Mbps, nRF24_TXPWR_12dBm, nRF24_CRC_2byte, nRF24_ARD_250us, 5 , nRF24_PIPE0, addr,nRF24_AA_ON  , 16);

  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
  //HAL_Delay(1);
  //HAL_SPI_Transmit(&hspi2, &send_spi_reg_addr, 1, 200);
  //HAL_SPI_Receive(&hspi2, &rec_spi_id_data, 1, 200);
  //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);



  uint8_t send_SPI_data[12] =  {
		    0x00, // CONFIG
		    0x01, // EN_AA
		    0x02, // EN_RXADDR
		    0x03, // SETUP_AW
		    0x04, // SETUP_RETR
		    0x05, // RF_CH
		    0x06, // RF_SETUP
		    0x07, // STATUS
		    0x08, // OBSERVE_TX
		    0x09, // RPD
		    0x0A, // RX_ADDR_P0 (first byte)
		    0x10, // TX_ADDR (first byte)
			};
  uint8_t received_SPI_data[12]= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//  uint8_t nrf24_read= {0x00};
  //nRF24_CSN_L();
 // HAL_Delay(1);
 // HAL_SPI_Transmit(&hspi1,send_SPI_data, 1, 200);
 // HAL_SPI_Receive(&hspi1,received_SPI_data, 1, 200);
  //nRF24_CSN_H();
  uint8_t test_data[]= "hi this is drone";
  nRF24_start_sending(test_data, 15);

    for(int x=0; x<12;x++){
        uint8_t tx_buf[2] = { send_SPI_data[x], 0xFF };  // Command and dummy byte
        uint8_t rx_buf[2] = { 0 };
        nRF24_CSN_L();
        HAL_Delay(1);
        HAL_SPI_TransmitReceive(&hspi1, tx_buf, rx_buf, 2, 200);
    	nRF24_CSN_H();
    	received_SPI_data[x] = rx_buf[1];
    }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  nRF24_start_sending(test_data, 15);
	    for(int x=0; x<12;x++){
	        uint8_t tx_buf[2] = { send_SPI_data[x], 0xFF };  // Command and dummy byte
	        uint8_t rx_buf[2] = { 0 };
	        nRF24_CSN_L();
	        HAL_Delay(1);
	        HAL_SPI_TransmitReceive(&hspi1, tx_buf, rx_buf, 2, 200);
	    	nRF24_CSN_H();
	    	received_SPI_data[x] = rx_buf[1];
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CE_Pin|CSN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CE_Pin CSN_Pin */
  GPIO_InitStruct.Pin = CE_Pin|CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void nRF24_set_up(uint8_t width, uint8_t channel, uint8_t data_rate, uint8_t power_level, uint8_t CRC_mode, uint8_t delay, uint8_t attempts, uint8_t rx_pipe, uint8_t *addr, uint8_t AA_state, uint8_t payload_size)
{
    // Set the address width (3, 4, or 5 bytes)
    nRF24_SetAddrWidth(width); // Recommended: 5-byte address width

    // Set the RF channel for communication (channel 0-125)
    nRF24_SetRFChannel(channel); // Set the channel (0-125)

    // Set the data rate (250kbps, 1Mbps, or 2Mbps)
    nRF24_SetDataRate(data_rate); // Set the data rate

    // Set the TX power level (available options depend on module, usually -18dBm to +3dBm)
    nRF24_SetTXPower(power_level); // Set the transmission power level

    // Set CRC mode (disable, 1-byte CRC, or 2-byte CRC)
    nRF24_SetCRCScheme(CRC_mode); // Set CRC scheme (disabled, CRC-1, or CRC-2)

    // Set auto-retransmit delay (ARD) and attempts (ARC)
    nRF24_SetAutoRetr(delay, attempts); // Configure retransmit delay and number of attempts (0-15)

    // Set the TX pipe address (used for transmission)
    nRF24_SetTXAddr(addr); // Set the TX pipe address

    // Set the RX pipe address (must match the TX address for auto acknowledgment)
    nRF24_SetAddr(rx_pipe, addr); // Set the RX pipe address (should match TX address)

    // Enable AA (Auto Acknowledgment) for the specified RX pipe
    nRF24_EnableAA(rx_pipe); // Enable AA on the given RX pipe (0-5)

    // Set the RX pipe with AA state (enabled/disabled) and payload size (1-32 bytes)
    nRF24_SetRXPipe(rx_pipe, AA_state, payload_size); // Configure RX pipe with AA and payload size

    nRF24_DisableAA(nRF24_PIPE1);

    nRF24_DisableAA(nRF24_PIPE2);

  	nRF24_DisableAA(nRF24_PIPE3);

    nRF24_DisableAA(nRF24_PIPE4);

    nRF24_DisableAA(nRF24_PIPE5);

    nRF24_SetPowerMode(nRF24_PWR_UP); // powering up module so that it can stay in standby-1 mode
    }

void nRF24_Switch_to_RX_mode(){      // set nrf24 into rx mode
	 nRF24_SetOperationalMode(nRF24_MODE_RX);
	 nRF24_CE_H();
};

typedef enum {
  NO_DATA,
  RX_RECEIVED
} nRF24_RX_Status;

nRF24_RX_Status RECEIVED_DATA(uint8_t *buffer, uint8_t *length) {   // this function checks for latest rx data and loads it into buffer and sets status.
    if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
        nRF24_ReadPayload(buffer, length);
        nRF24_ClearIRQFlags();
        return RX_RECEIVED;
    }
    return NO_DATA;
}

void nRF24_Switch_to_TX_mode(){
	 nRF24_SetOperationalMode(nRF24_MODE_TX);
	 nRF24_CE_H();
	 HAL_Delay(1);// TX_15us_delay() // Needs to be at least 10 μs
};



void nRF24_start_sending(uint8_t *data, uint8_t length) {
        nRF24_FlushTX();
        nRF24_ClearIRQFlags();
        nRF24_WritePayload(data, length);
        nRF24_Switch_to_TX_mode();
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
