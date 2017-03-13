/*
 * target_device.h
 *
 *  Created on: 13.03.2017
 *      Author: MPARTENF
 */

#ifndef APPLICATION_INC_TARGET_DEVICE_H_
#define APPLICATION_INC_TARGET_DEVICE_H_

#include "config.h"
#include "stdbool.h"

/**
 * Command ID defines start
 */
#define SINGLE_PING_ID		0x00 // INCOMING <--
#define BROADCAST_PING_ID	0x01 // INCOMING <--
#define PING_RESPONSE_ID	0x02 // OUTGOING   -->
#define INTERRUPT_ID		0x03 // INCOMING <--
#define GET_VERSION_ID		0x04 // INCOMING <--
#define SEND_VERSION_ID		0x05 // OUTGOING   -->
#define GET_NAME_ID			0x06 // INCOMING <--
#define SEND_NAME_ID		0x07 // OUTGOING   -->
#define INIT_FLASH_ID		0x08 // INCOMING <--
#define ACK_REQUEST_ID		0x09 // INCOMING <--
#define ACK_ID				0x0A // OUTGOING   -->
#define NACK_ID				0x0B // OUTGOING   -->
#define EXIT_FLASH_ID		0x0C // INCOMING <--
#define GET_CRC_ID			0x0D // INCOMING <--
#define SEND_CRC_ID			0x0E // OUTGOING   -->
#define START_FLASH_ID		0x0F // INCOMING <--
#define END_FLASH_ID		0x10 // INCOMING <--
#define START_APP_ID		0xFF // INCOMING <--
/**
 * Command ID defines end
 */

/**
 * Generic Message definition
 */
typedef struct
{
	bool FOF; // FlashOperationField, used when flash packs are being sent

	uint8_t targetDeviceId; // ID of the target device, always used

	uint32_t flashPackId; 	// ID of the flash pack, only used when FOF == 1

	uint8_t commandId; 		// Command ID of the message, used when FOF == 0

	uint8_t length;			// Length of the data array

	uint8_t data[8];		// Data array of the message
}BlGenericMessage;

/**
 * Function definitions
 */
void processBlMessage(BlGenericMessage* msg);
void sendPingResponse(BlGenericMessage* msg);
void enterBootMenu();
void sendVersion();
void sendName();
void initFlashMode();
void exitFlashMode();
void startFlashing();
void stopFlashing();
void sendError(uint8_t errCode);
void checkSprint();
void sendCRC();

#endif /* APPLICATION_INC_TARGET_DEVICE_H_ */
