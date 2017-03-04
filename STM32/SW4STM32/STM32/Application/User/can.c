#include "stm32f4xx_hal.h"


/**
 * CAN Abstraction Layer for CANnon Target Bootloader
 */

/**
 * Startup Message
 * This will be sent once on startup of the Bootloader
 */
void sendStartupMessage()
{

}


/**
 * Initializes the CAN Filter for the Bootloader
 */
void initCanFilter()
{

}

/**
 *  CAN1 init function
 *  Initializes the CAN1 timings
 */
static void MX_CAN1_Init(void)
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
