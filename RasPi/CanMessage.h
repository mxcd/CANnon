/*
 * CanMessage.h
 *
 *  Created on: Mar 16, 2017
 *      Author: max
 */

#ifndef CANMESSAGE_H_
#define CANMESSAGE_H_

typedef struct
{
	char dlc;
	long id;
	char data[8];

}CanMessage;

#endif /* CANMESSAGE_H_ */
