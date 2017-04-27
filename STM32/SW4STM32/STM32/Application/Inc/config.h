
/**
 * Bootloader Config
 */

/**
 * Common Includes
 */
#include "stdlib.h"
#include "stdbool.h"
#include "stm32f4xx_hal.h"

#define CAN_STARTUP_MSG_ID 0xFAU

/*------ CAN A1 ------*/
#define CHIP_ID 1U 	// COM_GW
//#define CHIP_ID 2U	// HMI_DB
//#define CHIP_ID 3U	// HVM_BMS
//#define CHIP_ID 4U	// HVM_ECU
//#define CHIP_ID 5U	// HVM_PM
//#define CHIP_ID 6U	// LVS_BMS
//#define CHIP_ID 7U	// LVS_ECU_F
//#define CHIP_ID 8U	// LVS_ECU_R1
//#define CHIP_ID 9U	// LVS_ECU_R2
//#define CHIP_ID 10U	// MS_AD_F
//#define CHIP_ID 11U	// MS_AD_PEDAL
//#define CHIP_ID 12U	// MS_9D_F
//#define CHIP_ID 13U	// MS_V2O
//#define CHIP_ID 14U	// VD_ECU
//#define CHIP_ID 21U	// MS_AD_OCPD

/*------ CAN A2 ------*/
//#define CHIP_ID 15U	// MS_AD_R
//#define CHIP_ID 16U	// MS_9D_R
//#define CHIP_ID 17U	// MS_TT_FL
//#define CHIP_ID 18U	// MS_TT_FR
//#define CHIP_ID 19U	// MS_TT_RL
//#define CHIP_ID 20U	// MS_TT_RR

