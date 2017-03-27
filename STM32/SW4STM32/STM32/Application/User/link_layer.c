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

void BL_RunUserApp(void) {

	extern pFunction JumpToApplication;
	extern uint32_t JumpAddress;

	//Disable all Interrupts (ST PM0214 p.210)
	NVIC->ICER[0] = (uint32_t)0xFFFFFFFF;
	NVIC->ICER[1] = (uint32_t)0xFFFFFFFF;
	NVIC->ICER[2] = (uint32_t)0xFFFFFFFF;
	//Disable all interrupt pending flags (ST PM0214 p.212)
	NVIC->ICPR[0] = (uint32_t)0xFFFFFFFF;
	NVIC->ICPR[1] = (uint32_t)0xFFFFFFFF;
	NVIC->ICPR[2] = (uint32_t)0xFFFFFFFF;

	SysTick->CTRL = 0;

	//Reset RCC clock
	RCC->CR |= (uint32_t)0x00000001;
	RCC->CFGR = (uint32_t)0x00000000;
	RCC->CR &= (uint32_t)0xFEF6FFFF;
	RCC->PLLCFGR = 0x24003010;
	RCC->CR &= (uint32_t)0xFFFBFFFF;
	RCC->CIR = 0x00000000;
	FLASH->ACR = 0;


	__ISB();
	__DSB();

	//basic test for valid application vector table start
	if ((*(__IO uint32_t*)BL_UAStartAddress) == BL_UAStartPattern) {
		// point to application reset handler (APPLICATION_ADDRESS +4 bytes)
		JumpAddress = *(__IO uint32_t*) (BL_UAStartAddress + 4);
		JumpToApplication = (pFunction) JumpAddress;

		// update main stack pointer (MSP) register value
		__set_MSP(*(__IO uint32_t*) BL_UAStartAddress);

		// execute application code
		JumpToApplication();
	}
}

uint64_t getDeviceId()
{
	uint64_t devId;
	devId = (uint64_t) *GTBL_STM32Addr_UUIDLow;
	devId += ((uint64_t)*GTBL_STM32Addr_UUIDMid) << 32;
	return devId;
}
