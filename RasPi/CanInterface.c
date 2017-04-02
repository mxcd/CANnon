/*
 * CanInterface.cpp
 *
 *  Created on: Mar 16, 2017
 *      Author: max
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "lib.h"

#include "CanMessage.h"
#include "CanInterface.h"
#include "config.h"



void initCanInterface(int deviceId)
{
	int baseSocket;
	struct sockaddr_can addr;
	struct ifreq ifr;

	baseSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);

	strcpy(ifr.ifr_name, "can0" );
	ioctl(baseSocket, SIOCGIFINDEX, &ifr);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	//struct can_filter *rfilter;
	//rfilter = malloc(sizeof(struct can_filter));


	struct can_filter rfilter;

	if(deviceId != 0)
	{
		rfilter.can_id = CAN_EFF_FLAG | deviceId << 20;
		rfilter.can_mask = CAN_EFF_FLAG | 0xFF << 20;
	}
	else
	{
		rfilter.can_id = CAN_EFF_FLAG;
		rfilter.can_mask = CAN_EFF_FLAG;
	}


	setsockopt(baseSocket, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

	bind(baseSocket, (struct sockaddr *)&addr, sizeof(addr));
}

bool canAvailable()
{
	FD_ZERO(&rdfs);
	FD_SET(baseSocket, &rdfs);
	return (FD_ISSET(baseSocket, &rdfs));
}

int receiveMessage(CanMessage* cMsg)
{
	struct can_frame frame;
	int nbytes;
	nbytes = read(baseSocket, &frame, sizeof(struct can_frame));

	if (nbytes < 0)
	{
		perror("can raw socket read");
		return 1;
	}

	/* paranoid check ... */
	if (nbytes < sizeof(struct can_frame))
	{
		fprintf(stderr, "read: incomplete CAN frame\n");
		return 1;
	}
	return cMsg;
}

BlGenericMessage receiveGenericMessage(BlGenericMessage* msg)
{
	CanMessage cMsg;
	receiveMessage(&cMsg);

	BlGenricMessage msg = msgToGeneric(&cMsg);

	return msg;
}

void printErrorCodes(BlGenericMessage* msg)
{
	if(__VERBOSE)
		if(msg->commandId == STATUS_ID)
		{
			switch(msg.data[0])
			{
			case ERRCODE_NO_FLASH_PROCESS:
				printf("Error: No flashing in process!\n");
				break;
			case ERRCODE_NOT_IN_FLASH_MODE:
				printf("Error: Not in flash mode\n");
				break;
			case ERRCODE_NOT_IN_BOOT_MENU:
				printf("Error: Not in boot menu\n");
				break;
			case ERRCODE_ALREADY_FLASHING:
				printf("Error: Already flashing\n");
				break;
			case STATUS_IN_BOOT_MENU:
				printf("Status: Entering boot menu\n");
				break;
			case STATUS_START_ERASE:
				printf("Status: Starting flash erase\n");
				break;
			case STATUS_ERASE_FINISHED:
				printf("Status: Flash erase finished\n");
				break;
			case STATUS_FLASH_START:
				printf("Status: Starting flashing\n");
				break;
			case STATUS_FLASH_DONE:
				printf("Status: Flashing done\n");
				break;
			case STATUS_STARTING_APP:
				printf("Status: Starting user app\n");
				break;
			}
		}
}

void sendGenericMessage(BlGenericMessage* msg)
{
	int id = ((msg->FOF & 0x1) << 28);
	id |= (msg->targetDeviceId & 0xFF) << 20;
	if(msg->FOF)
	{
		id |= (msg->flashPackId & 0xFFFFF);
	}
	else
	{
		id |= (msg->commandId & 0xFF) << 12;
	}

	CanMessage cMsg;
	cMsg.dlc = msg->length;
	cMsg.id = id;
	cMsg.ext = 1;
	int i;
	for(i = 0; i < msg->length; ++i)
	{
		cMsg.data[i] = msg->data[i];
//		printf("%i:%x\n", i, msg->data[i]);
	}
	sendMessage(&cMsg);
}

void sendMessage(CanMessage* msg)
{
	int nbytes;
	int i;
	struct can_frame frame;
	frame.can_dlc = msg->dlc;
	frame.can_id = msg->id;
//	printf("Sending msg /w id %x\n", msg->id);
	if(msg->ext)
	{
		frame.can_id |= CAN_EFF_FLAG;
	}
	for(i = 0; i < msg->dlc; ++i)
	{
		frame.data[i] = msg->data[i];
	}

	if ((nbytes = write(baseSocket, &frame, sizeof(frame))) != sizeof(frame))
	{
		perror("write");
	}
}

static void msgToGeneric(CanMessage* cMsg, BlGenericMessage* msg)
{
	msg->FOF = ((cMsg->id>>28)&0x1);
	msg->commandId = (cMsg->id>>12)&0xFF;
	msg->targetDeviceId = (cMsg->id>>20)&0xFF;
	msg->length = cMsg->dlc;
	int i;
	for(i = 0; i < msg->length; ++i)
	{
		msg->data[i] = cMsg->data[i];
	}
}
