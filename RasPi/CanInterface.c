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
#include <fcntl.h>

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

	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, "can0" );
	ioctl(baseSocket, SIOCGIFINDEX, &ifr);
	addr.can_ifindex = ifr.ifr_ifindex;

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

	setsockopt(baseSocket, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(struct can_filter));

	bind(baseSocket, (struct sockaddr *)&addr, sizeof(addr));
	//SetSocketBlockingEnabled(baseSocket, false);

	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;
}

int receiveMessage(CanMessage* cMsg)
{
	int i;
	int count, nbytes = 0;
	struct cmsghdr *cmsg;

	FD_ZERO(&rdfs);
	FD_SET(baseSocket, &rdfs);
	if(FD_ISSET(baseSocket, &rdfs))
	{
		if(__VERBOSE)
			printf("cMsg receive\n");
		fflush(stdout);
		struct can_frame frame;
		int nbytes, i;
		if(__VERBOSE)
			printf("socket read\n");

		//nbytes = read(baseSocket, &frame, sizeof(struct can_frame));
		nbytes = recvfrom(baseSocket, &frame, sizeof(struct can_frame), 0, NULL, NULL);

		if(__VERBOSE)
			printf("socket read done\n");

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

		for(i = 0; i < frame.can_dlc; ++i)
		{
			cMsg->data[i] = frame.data[i];
		}
		cMsg->dlc = frame.can_dlc;
		cMsg->ext = true;
		cMsg->id = frame.can_id;
	}

	return nbytes;
}

int receiveGenericMessage(BlGenericMessage* msg)
{
	if(__VERBOSE)
		printf("msg receive\n");
	fflush(stdout);
	CanMessage cMsg;
	int nbytes;

	nbytes = receiveMessage(&cMsg);
	msgToGeneric(&cMsg, msg);

	printErrorCodes(msg);

	return nbytes;
}

void printErrorCodes(BlGenericMessage* msg)
{
	if(__VERBOSE)
	{
		if(msg->commandId == STATUS_ID)
		{
			switch(msg->data[0])
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
}

void sendGenericMessage(BlGenericMessage* msg)
{
	printf("msg send");
	fflush(stdout);
	CanMessage cMsg;
	genericToMsg(msg, &cMsg);
	sendMessage(&cMsg);
}

void sendMessage(CanMessage* msg)
{
	printf("cMsg send");
	fflush(stdout);
	int nbytes;
	int i;
	struct can_frame frame;
	frame.can_dlc = msg->dlc;
	frame.can_id = msg->id;

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

static void genericToMsg(BlGenericMessage* msg, CanMessage* cMsg)
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

	cMsg->dlc = msg->length;
	cMsg->id = id;
	cMsg->ext = 1;
	int i;
	for(i = 0; i < msg->length; ++i)
	{
		cMsg->data[i] = msg->data[i];
	}
}

/** Returns true on success, or false if there was an error */
bool SetSocketBlockingEnabled(int fd, bool blocking)
{
   if (fd < 0) return false;

	#ifdef WIN32
	   unsigned long mode = blocking ? 0 : 1;
	   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
	#else
	   int flags = fcntl(fd, F_GETFL, 0);
	   if (flags < 0) return false;
	   flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
	   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
	#endif
}

