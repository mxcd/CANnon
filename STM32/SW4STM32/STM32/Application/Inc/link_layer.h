/*
 * link_layer.h
 *
 *  Created on: 13.03.2017
 *      Author: MPARTENF
 */

#ifndef APPLICATION_INC_LINK_LAYER_H_
#define APPLICATION_INC_LINK_LAYER_H_

/**
 * LinkLayer for Message Transfer
 * All STM32 specific
 */
#include "config.h"

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

// The function pointer to the message processor
//void (*linkLayermessageProcessor)(BlGenericMessage* msg);

typedef  void (*pFunction)(void);

// Assign default Chip ID
uint8_t chipID;

void initDevice();
void deinitDevice();
void jumpToUserApp();

void sendGenericMessage(BlGenericMessage* msg);

void unlockFlash();
void lockFlash();
void writeByteToFlash(uint8_t data, uint32_t position);
void writeMessageToFlash(uint8_t* data, uint32_t position);

uint64_t getDeviceId();

/**
 * Helper functions
 */
void setChipId();

#endif /* APPLICATION_INC_LINK_LAYER_H_ */
