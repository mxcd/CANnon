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

#endif /* CANMESSAGE_H_ */
