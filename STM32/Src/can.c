/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "can.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "config.h"
#include "target_device.h"



void can1Init()
{
	extern CanRxMsgTypeDef messageRx;
	hcan1.pRxMsg = &messageRx;
	can1InitFilterMask();
	HAL_CAN_Receive_IT(&hcan1, CAN_FILTER_FIFO0);
}


/**
 * Sends out and CAN message on CAN bus 1
 * @param data: The (max 8 Byte) data array to be sent
 * @param length: The length of the data array
 * @param id: CAN Message ID
 * @param extID: Extended Identifier (23bit) will be used when true, else Standard Identifier (11bit); default = false
 */
void can1SendStd(uint8_t* data, uint8_t length, uint32_t id)
{
	CanTxMsgTypeDef canTxMsg;
	hcan1.pTxMsg = &canTxMsg;
	hcan1.pTxMsg->StdId = id & 0x7FFU;
	hcan1.pTxMsg->ExtId = 0x01;
	hcan1.pTxMsg->RTR = CAN_RTR_DATA;
	hcan1.pTxMsg->IDE = CAN_ID_STD;

	hcan1.pTxMsg->DLC = length;

	uint8_t i;
	for(i = 0; i < length; i++)
	{
		hcan1.pTxMsg->Data[i] = data[i];
	}

	HAL_CAN_Transmit_IT(&hcan1);
}

/**
 * Function overload for can1Send with extId
 */
void can1SendExt(uint8_t* data, uint8_t length, uint32_t id)
{
	CanTxMsgTypeDef canTxMsg;
	hcan1.pTxMsg = &canTxMsg;
	hcan1.pTxMsg->StdId = 0x01;
	hcan1.pTxMsg->ExtId = id & 0x1FFFFFFFU;
	hcan1.pTxMsg->RTR = CAN_RTR_DATA;
	hcan1.pTxMsg->IDE = CAN_ID_EXT;

	hcan1.pTxMsg->DLC = length;

	uint8_t i;
	for(i = 0; i < length; i++)
	{
		hcan1.pTxMsg->Data[i] = data[i];
	}

	HAL_CAN_Transmit_IT(&hcan1);
}

void sendStartupMessage()
{
	canData[0] = CHIP_ID;
	can1SendExt(canData, 1, CAN_STARTUP_MSG_ID);
}

void can1InitFilterMask()
{
	// The following filters are required:
	// * Filter masked for target device id
	// * Filter for broadcast ping

	// [10:3][2:0 17:13][12:5][4:0 IDE RTR 0]
	// [28:21][20:13]||[12:5][4:0 IDE RTR 0]
	// 0000 0000 0000 0000 || 0000 0000 0000 0000
	CAN_FilterConfTypeDef canFilter;
	canFilter.FilterIdHigh = (chipID << 7);
	canFilter.FilterIdLow = 0x0000;
	canFilter.FilterMaskIdHigh = 0x7F80;
	canFilter.FilterMaskIdLow = 0x0000;
	canFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	canFilter.FilterNumber = 0;
	canFilter.BankNumber = 28;
	canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
	canFilter.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan1, &canFilter);

	// [28:21][20:13]||[12:5][4:0 IDE RTR 0]
	canFilter.FilterIdHigh = ((BROADCAST_PING_ID >> 1) & 0x7F);
	canFilter.FilterIdLow = ((BROADCAST_PING_ID << 15) & 0x8000);
	canFilter.FilterMaskIdHigh = 0x007F;
	canFilter.FilterMaskIdLow = 0x8000;
	canFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	canFilter.FilterNumber = 1;
	canFilter.BankNumber = 28;
	canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
	canFilter.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan1, &canFilter);
}

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SJW = CAN_SJW_1TQ;
  hcan1.Init.BS1 = CAN_BS1_9TQ;
  hcan1.Init.BS2 = CAN_BS2_4TQ;
  hcan1.Init.TTCM = DISABLE;
  hcan1.Init.ABOM = ENABLE;
  hcan1.Init.AWUM = ENABLE;
  hcan1.Init.NART = DISABLE;
  hcan1.Init.RFLM = DISABLE;
  hcan1.Init.TXFP = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
  
    /**CAN1 GPIO Configuration    
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN1 GPIO Configuration    
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

  }
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
