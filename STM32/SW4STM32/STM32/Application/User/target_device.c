
/*
 * target_device.c
 *
 *  Created on: 13.03.2017
 *      Author: MPARTENF
 */

/**
 * Bootloader target device
 */
#include "target_device.h"

/**
 * Big Switch-Case Method for splitting up based on the Command ID
 */
void processBlMessage(BlGenericMessage* msg)
{
	if(msg->FOF)
	{
		// This is a flash pack message
		tryToWriteFlash(msg);
	}
	else
	{
		// This is NOT a flash pack message
		// Therefore we switch for the command id
		switch(msg->commandId)
		{
		case SINGLE_PING_ID:
		case BROADCAST_PING_ID:
			sendPingResponse(msg);
			break;
		case INTERRUPT_ID:
			enterBootMenu();
			break;
		case GET_VERSION_ID:
			sendVersion();
			break;
		case GET_NAME_ID:
			sendName();
			break;
		case INIT_FLASH_ID:
			initFlashMode();
			break;
		case ACK_REQUEST_ID:
			checkSprint();
			break;
		case EXIT_FLASH_ID:
			exitFlashMode();
			break;
		case GET_CRC_ID:
			sendCRC();
			break;
		case START_FLASH_ID:
			startFlashing(msg);
			break;
		case END_FLASH_ID:
			stopFlashing();
			break;
		case GET_CHIP_ID:
			sendChipId();
			break;
		case START_APP_ID:
			startApplication();
			break;
		}
	}
}

/**
 * Appends own device ID to data field and sends out ping response
 */
void sendPingResponse(BlGenericMessage* msg)
{
	msg->data[1] = chipID;
	msg->length = 2;
	sendGenericMessage(msg);
}

/**
 * Stops the timeout, entering the boot menu
 */
void enterBootMenu()
{
	// TODO send message to signal entering boot menu
	bootMenu = true;
	flashMode = false;
	inFlashProcess = false;
}

/**
 * Sends the version of current software
 */
void sendVersion()
{
	// TODO
}

/**
 * Sends the name of the current software
 */
void sendName()
{
	// TODO
}

/**
 * Enters the flash mode of the bootloader
 */
void initFlashMode()
{
	if(bootMenu)
	{
		flashMode = true;
		bootMenu = false;
		//lockFlash();
	}
	else
	{
		// Trying to go in flash mode without being in boot menu -> ERROR
		sendError(ERRCODE_NOT_IN_BOOT_MENU);
	}
}

/**
 * Quit the above entered flash mode
 */
void exitFlashMode()
{
	if(flashMode)
	{
		flashMode = false;
		bootMenu = true;
		lockFlash();
	}
	else
	{
		// Trying to leave flash mode without being in flash mode -> ERROR
		sendError(ERRCODE_NOT_IN_FLASH_MODE);
	}
}

/**
 * Start the flashing process. Only now flash packs are allowed.
 */
void startFlashing(BlGenericMessage* msg)
{
	if(flashMode)
	{
		if(!inFlashProcess)
		{
			inFlashProcess = true;
			uint8_t i;
			uint64_t data = 0;
			for(i = 0; i < msg->length; ++i)
			{
				data += msg->data[i] << (i*8);
			}
			packCount = data & 0xFFFFF;
			sprintPackCount = (data>>24) & 0xFF;
			packCounter = 0;
			sprintPackCounter = 0;
			sprintFlags = 0;
		}
		else
		{
			// Already in flash process -> ERROR
			sendError(ERRCODE_ALREADY_FLASHING);
		}
	}
	else
	{
		// Trying to start flash process without being in flash mode -> ERROR
		sendError(ERRCODE_NOT_IN_FLASH_MODE);
	}
}

/**
 * Stop the flashing. No more flash packs allowed.
 */
void stopFlashing()
{
	if(inFlashProcess)
	{
		inFlashProcess = false;
	}
	else
	{
		// Trying to stop the flash process without being in flash process -> ERROR
		sendError(ERRCODE_NO_FLASH_PROCESS);
	}
}

/**
 * Sends out and error message with defined error code
 */
void sendError(uint8_t errCode)
{
	BlGenericMessage msg;
	msg.FOF = 0;
	msg.commandId = ERROR_ID;
	msg.data[0] = errCode;
	msg.flashPackId = 0;
	msg.length = 1;
	msg.targetDeviceId = chipID;
	sendGenericMessage(&msg);
}

/**
 * Checks if the last sprint of flash packs has arrived properly
 */
void checkSprint()
{
	// Check if all required packs arrived
	bool complete =  (sprintFlags & ((1u << sprintPackCount) - 1)) == (1u << sprintPackCount) - 1;

	if(complete && sprintPackCount == sprintPackCounter)
	{
		// If yes: Send ACK
		sendAck();
		sprintFlags = 0;
		sprintPackCounter = 0;

	}
	else
	{
		// If no: Send NACK with bit array of received packs
		sendNack(sprintFlags);
	}
}

void sendAck()
{
	BlGenericMessage msg;
	msg.FOF = 0;
	msg.commandId = 0x0A;
	msg.flashPackId = 0;
	msg.targetDeviceId = chipID;
	msg.length = 0;

	sendGenericMessage(&msg);
}

void sendNack(uint64_t sprintFlags)
{
	BlGenericMessage msg;
	msg.FOF = 0;
	msg.commandId = 0x0B;
	msg.flashPackId = 0;
	msg.length = 8;
	msg.targetDeviceId = chipID;
	uint8_t i;
	for(i = 0; i < 8; ++i)
	{
		msg.data[i] = sprintFlags >> (i*8);
	}

	sendGenericMessage(&msg);
}

/**
 * Send the CRC checksum that has been generated while flashing
 */
void sendCRC()
{
	// TODO
}

/**
 * Sends the first 64bit of the UUID Chip ID
 */
void sendChipId()
{
	uint64_t devId = getDeviceId();
	BlGenericMessage msg;
	msg.FOF = 0;
	msg.commandId = SEND_CHIP_ID;
	msg.flashPackId = 0;
	msg.length = 8;
	msg.targetDeviceId = chipID;

	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		msg.data[i] = devId >> (i*8);
	}
	sendGenericMessage(&msg);
}

/**
 * Calls the de-Init and jumps to the user application
 */
void startApplication()
{
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
	deinitDevice();
	jumpToUserApp();
}

/**
 * Checks if the required conditions are met, then forwards the flash data to the link layer
 */
void tryToWriteFlash(BlGenericMessage* msg)
{
	if(flashMode)
	{
		if(inFlashProcess)
		{
			// Process flash pack
			uint32_t packId = msg->flashPackId;					// global id of the pack
			uint8_t sprintPosition = packId % sprintPackCount;	// position of pack in sprint

			//Set the corresponding bit in the sprint flags
			sprintFlags |= (1<<sprintPosition);

			// Write pack to flash, calculating CRC alongside
			writeMessageToFlash(msg->data, (packId*8), msg->length);

			++packCounter;
			++sprintPackCounter;
		}
		else
		{
			// Trying to write flash without being in flash process -> ERROR
			sendError(ERRCODE_NO_FLASH_PROCESS);
		}
	}
	else
	{
		// Trying to write flash without being in flash mode -> ERROR
		sendError(ERRCODE_NOT_IN_FLASH_MODE);
	}
}
