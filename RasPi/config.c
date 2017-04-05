/*
 * config.c
 *
 *  Created on: Apr 5, 2017
 *      Author: max
 */

#include "config.h"

/**
 * Definition of Target Devices
 */
void initTargetDeviceConfig()
{
	blconf[0].id = 1;
	blconf[0].name = "COM_GW";
	blconf[0].LVS = LVS_R1;
	blconf[0].CH = 8;

	blconf[1].id = 2;
	blconf[1].name = "HMI_DB";
	blconf[1].LVS = LVS_F;
	blconf[1].CH = 7;

	blconf[2].id = 3;
	blconf[2].name = "HVM_BMS";
	blconf[2].LVS = LVS_R2;
	blconf[2].CH = 1;

	blconf[3].id = 4;
	blconf[3].name = "HVM_ECU";
	blconf[3].LVS = LVS_R1;
	blconf[3].CH = 2;

	blconf[4].id = 5;
	blconf[4].name = "HVM_PM";
	blconf[4].LVS = LVS_R1;
	blconf[4].CH = 4;

	blconf[5].id = 6;
	blconf[5].name = "LVS_BMS";
	blconf[5].LVS = LVS_NONE;
	blconf[5].CH = -1;

	blconf[6].id = 7;
	blconf[6].name = "LVS_ECU_F";
	blconf[6].LVS = LVS_BMS;
	blconf[6].CH = 0;

	blconf[7].id = 8;
	blconf[7].name = "LVS_ECU_R1";
	blconf[7].LVS = LVS_BMS;
	blconf[7].CH = 0;

	blconf[8].id = 9;
	blconf[8].name = "LVS_ECU_R2";
	blconf[8].LVS = LVS_BMS;
	blconf[8].CH = 0;

	blconf[9].id = 10;
	blconf[9].name = "MS_AD_F";
	blconf[9].LVS = LVS_F;
	blconf[9].CH = 5;

	blconf[10].id = 11;
	blconf[10].name = "MS_AD_PEDALS";
	blconf[10].LVS = LVS_F;
	blconf[10].CH = 1;

	blconf[11].id = 12;
	blconf[11].name = "MS_9D_F";
	blconf[11].LVS = LVS_F;
	blconf[11].CH = 4;

	blconf[12].id = 13;
	blconf[12].name = "MS_V2O";
	blconf[12].LVS = LVS_F;
	blconf[12].CH = 6;

	blconf[13].id = 14;
	blconf[13].name = "VD_ECU";
	blconf[13].LVS = LVS_R2;
	blconf[13].CH = 6;

	blconf[14].id = 15;
	blconf[14].name = "MS_AD_R";
	blconf[14].LVS = LVS_R2;
	blconf[14].CH = 8;

	blconf[15].id = 16;
	blconf[15].name = "MS_9D_R";
	blconf[15].LVS = LVS_R2;
	blconf[15].CH = 3;

	blconf[16].id = 17;
	blconf[16].name = "MS_TT_FL";
	blconf[16].LVS = LVS_F;
	blconf[16].CH = 0;

	blconf[17].id = 18;
	blconf[17].name = "MS_TT_FR";
	blconf[17].LVS = LVS_F;
	blconf[17].CH = 0;

	blconf[18].id = 19;
	blconf[18].name = "MS_TT_RL";
	blconf[18].LVS = LVS_R1;
	blconf[18].CH = 4;

	blconf[19].id = 20;
	blconf[19].name = "MS_TT_RR";
	blconf[19].LVS = LVS_R1;
	blconf[19].CH = 4;

	blconf[20].id = 21;
	blconf[20].name = "MS_AD_OCPD";
	blconf[20].LVS = LVS_R1;
	blconf[20].CH = 2;

	maxDeviceCount = 21;

	//#define CHIP_ID 1U 	// COM_GW 		// R1 8
	//#define CHIP_ID 2U	// HMI_DB 		// F 2
	//#define CHIP_ID 3U	// HVM_BMS 		// R2 1
	//#define CHIP_ID 4U	// HVM_ECU 		// R1 2
	//#define CHIP_ID 5U	// HVM_PM  		// R1 6
	//#define CHIP_ID 6U	// LVS_BMS
	//#define CHIP_ID 7U	// LVS_ECU_F
	//#define CHIP_ID 8U	// LVS_ECU_R1
	//#define CHIP_ID 9U	// LVS_ECU_R2
	//#define CHIP_ID 10U	// MS_AD_F 		// F 5
	//#define CHIP_ID 11U	// MS_AD_PEDAL 	// F 1
	//#define CHIP_ID 12U	// MS_9D_F 		// F 4
	//#define CHIP_ID 13U	// MS_V2O 		// F 6
	//#define CHIP_ID 14U	// VD_ECU 		// R2 6
	/*------ CAN A2 ------*/
	//#define CHIP_ID 15U	// MS_AD_R		// R2 8
	//#define CHIP_ID 16U	// MS_9D_R		// R2 3
	//#define CHIP_ID 17U	// MS_TT_FL 	// F 0
	//#define CHIP_ID 18U	// MS_TT_FR 	// F 0
	//#define CHIP_ID 19U	// MS_TT_RL 	// R1 4
	//#define CHIP_ID 20U	// MS_TT_RR 	// R1 4
}
/**
 * End Definition of Target Devices
 */


