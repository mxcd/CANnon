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
			startFlashing();
			break;
		case END_FLASH_ID:
			stopFlashing();
			break;
		case START_APP_ID:
			startApplication();
			break;
		}
	}

	/**
	 * Appends own device ID to data field and sends out ping response
	 */
	void sendPingResponse(BlGenericMessage* msg)
	{

	}

	/**
	 * Stops the timeout, entering the boot menu
	 */
	void enterBootMenu()
	{

	}

	/**
	 * Sends the version of current software
	 */
	void sendVersion()
	{

	}

	/**
	 * Sends the name of the current software
	 */
	void sendName()
	{

	}

	/**
	 * Enters the flash mode of the bootloader
	 */
	void initFlashMode()
	{

	}

	/**
	 * Quit the above entered flash mode
	 */
	void exitFlashMode()
	{

	}

	/**
	 * Start the flashing process. Only now flash packs are allowed.
	 */
	void startFlashing()
	{

	}

	/**
	 * Stop the flashing. No more flash packs allowed.
	 */
	void stopFlashing()
	{

	}

	/**
	 * Sends out and error message with defined error code
	 */
	void sendError(uint8_t errCode)
	{

	}

	/**
	 * Checks if the last sprint of flash packs has arrived properly
	 */
	void checkSprint()
	{
		// Check if all required pacs arrived
		// If yes: Send ACK
		// If no: Send NACK with bit array of received packs
	}

	/**
	 * Send the CRC checksum that has been generated while flashing
	 */
	void sendCRC()
	{

	}
}
