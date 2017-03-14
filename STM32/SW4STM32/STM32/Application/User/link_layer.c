/*
 * link_layer.c
 *
 *  Created on: 14.03.2017
 *      Author: MPARTENF
 */

#include "link_layer.h"


#define GTBL_STM32Addr_UUIDHigh		(uint32_t*) 0x1FFF7A18		//32bit
#define GTBL_STM32Addr_UUIDMid		(uint32_t*) 0x1FFF7A14		//32bit
#define GTBL_STM32Addr_UUIDLow		(uint32_t*) 0x1FFF7A10		//32bit
#define GTBL_STM32Addr_FlashSize	(uint32_t*) 0x1FFF7A22		//16bit

#define GTBL_AppStartAddress	0x08200000
#define	GTBL_AppLen				960			//KByte
#define GTBL_AppEndAddress		GTBL_AppStartAddress + GTBL_AppLen -4


void initMessageInterface()
{
	// TODO
}

void deinitMessageInterface()
{
	// TODO
}

/**
 * Sends out the generic message via the LinkLayer Specific Physical Layer
 */
void sendGenericMessage(BlGenericMessage* msg)
{
	// TODO
}

/**
 * Writes a single byte to the flash memory
 * @param data: The data to be written to the flash memory
 * @param position: The position in flash
 */
void writeToFlash(uint8_t data, uint32_t position)
{
	// TODO
}

/**
 * Jumps to the start of the user application
 */
void jumpToUserApp()
{
	// TODO Better let Ecki check that
	extern pFunction GTBL_JumpToApplication;
	extern uint32_t GTBL_JumpAddress;

	/* Jump to user application */
	GTBL_JumpAddress = *(__IO uint32_t*) (GTBL_AppStartAddress + 4);
	GTBL_JumpToApplication = (pFunction) GTBL_JumpAddress;
	/* Initialize user application's Stack Pointer */
	__set_MSP(*(__IO uint32_t*) GTBL_AppStartAddress);
	GTBL_JumpToApplication();
}

uint64_t getDeviceId()
{
	uint64_t devId;
	devId = (uint64_t) *GTBL_STM32Addr_UUIDLow;
	devId += ((uint64_t)*GTBL_STM32Addr_UUIDMid) << 32;
	return devId;
}
