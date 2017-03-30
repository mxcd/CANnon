/*
 * link_layer.c
 *
 *  Created on: 14.03.2017
 *      Author: MPARTENF
 */

#include "link_layer.h"
#include "can.h"
#include "stm32f4xx_hal.h"



#define GTBL_STM32Addr_UUIDHigh		(uint32_t*) 0x1FFF7A18		//32bit
#define GTBL_STM32Addr_UUIDMid		(uint32_t*) 0x1FFF7A14		//32bit
#define GTBL_STM32Addr_UUIDLow		(uint32_t*) 0x1FFF7A10		//32bit
#define GTBL_STM32Addr_FlashSize	(uint32_t*) 0x1FFF7A22		//16bit

#define GTBL_AppStartAddress	0x08010000
#define	GTBL_AppLen				960			//KByte
#define GTBL_AppEndAddress		GTBL_AppStartAddress + GTBL_AppLen -4


void initDevice()
{
	setChipId();
	can1Init();
}

void deinitDevice()
{
	HAL_CAN_MspDeInit(&hcan1);
}

/**
 * Sends out the generic message via the LinkLayer Specific Physical Layer
 */
void sendGenericMessage(BlGenericMessage* msg)
{
	// Build Message ID
	// [FOF == 1][Target ID][Flash Pack ID]
	// [   28   ][ 27 - 20 ][    19 - 0   ]
	//
	// [FOF == 0][Target ID][CMD ID]
	// [   28   ][ 27 - 20 ][ 19-12]

	uint32_t id = ((msg->FOF & 0x1) << 28);
	id |= (msg->targetDeviceId & 0xFF) << 20;
	if(msg->FOF)
	{
		id |= (msg->flashPackId & 0xFFFFF);
	}
	else
	{
		id |= (msg->commandId & 0xFF) << 12;
	}

	can1SendExt(msg->data, msg->length, id);
}



/**
 * Unlocks the flash memory, so that it is accessible and can be written
 */
void unlockFlash()
{
	HAL_FLASH_Unlock();
}

void clearUserAppFlash()
{
	FLASH_EraseInitTypeDef eraseDef;
	eraseDef.TypeErase = TYPEERASE_SECTORS;
	eraseDef.Sector = FLASH_SECTOR_4;
	eraseDef.NbSectors = 8;
	eraseDef.VoltageRange = VOLTAGE_RANGE_3;

	uint32_t eraseOutput;

	HAL_FLASHEx_Erase(&eraseDef, &eraseOutput);
	//uint8_t bank;
	//for(bank = 4; bank <= 11; bank++)
	//{
		/*while(READ_REG(0x0C)&0x1);

		FLASH->CR &= 0xFFFFFC85;
		FLASH->CR |= (bank<<3);
		FLASH->CR |= 2;
		FLASH->CR |= (1<<16);
		*/
		//HAL_Delay(10);
		//while(READ_REG(0x0C)&0x1);
		//FLASH_Erase_Sector(bank, FLASH_VOLTAGE_RANGE_3);
	//}
	//while(READ_REG(0x0C)&0x1);
}

/**
 * Locks the flash after successful falsh operation again
 */
void lockFlash()
{
	HAL_FLASH_Lock();
}

/**
 * Writes a 8 byte block to the flash memory
 * @param data: The data to be written to the flash memory
 * @param position: The byte position (start) in flash
 */
void writeMessageToFlash(uint8_t* data, uint32_t position, uint8_t length)
{
	uint32_t flashBuffer;
	uint8_t i;
	uint8_t j;

	if(length != 0)
	{
		// Code for 32 Bit addressing
		/*for(i = 0; i < ((length-1/4) + 1); ++ i)
		{
			flashBuffer = 0;
			for(j = 0; j < 4; ++i)
			{
				flashBuffer |= data[j] << (j*8);
			}
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, GTBL_AppStartAddress + position + i, flashBuffer);
		}*/

		for(i = 0; i < ((length-1/4) + 1); i++)
		{
			flashBuffer = 0;
			for(j = 0; j < 4; j++)
			{
				flashBuffer |= data[j] << (j*8);
			}
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, GTBL_AppStartAddress + position + i*4, flashBuffer);
		}
	}
}

/**
 * Jumps to the start of the user application after deinit interrupts.
 */
void jumpToUserApp(void) {

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

	// TODO: ask binder. Until then: ignore check
	//if ((*(__IO uint32_t*)GTBL_AppStartAddress) == BL_UAStartPattern)
	{
		// point to application reset handler (APPLICATION_ADDRESS +4 bytes)
		JumpAddress = *(__IO uint32_t*) (GTBL_AppStartAddress + 4);
		JumpToApplication = (pFunction) JumpAddress;

		// update main stack pointer (MSP) register value
		__set_MSP(*(__IO uint32_t*) GTBL_AppStartAddress);

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

void setChipId()
{
	chipID = (uint8_t)getDeviceId();
}
