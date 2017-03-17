/*
 * CanMessage.h
 *
 *  Created on: Mar 16, 2017
 *      Author: max
 */

#ifndef CANMESSAGE_H_
#define CANMESSAGE_H_
#include <stdbool.h>

typedef struct
{
	char dlc;
	long id;
	char data[8];
	bool ext;

}CanMessage;

typedef struct
{
	bool FOF; // FlashOperationField, used when flash packs are being sent

	int targetDeviceId; // ID of the target device, always used

	int flashPackId; 	// ID of the flash pack, only used when FOF == 1

	int commandId; 		// Command ID of the message, used when FOF == 0

	int length;			// Length of the data array

	char data[8];		// Data array of the message
}BlGenericMessage;

#endif /* CANMESSAGE_H_ */
