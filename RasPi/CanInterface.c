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
	/* open socket */
	if ((baseSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{
		perror("socket");
	}

	addr.can_family = AF_CAN;

	strcpy(ifr.ifr_name, "can0");
	if (ioctl(baseSocket, SIOCGIFINDEX, &ifr) < 0)
	{
		perror("SIOCGIFINDEX");
	}
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


	setsockopt(baseSocket, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(struct can_filter));

	//free(rfilter);

	if (bind(baseSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
	}

	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;
}

bool canAvailable()
{
	FD_ZERO(&rdfs);
	FD_SET(baseSocket, &rdfs);
	return (FD_ISSET(baseSocket, &rdfs));
}

CanMessage receiveMessage()
{
	int nbytes, i;
	int count = 0;
	struct cmsghdr *cmsg;

	CanMessage cMsg;

	FD_ZERO(&rdfs);
	FD_SET(baseSocket, &rdfs);
	if(FD_ISSET(baseSocket, &rdfs))
	{
		int idx;

		/* these settings may be modified by recvmsg() */
		iov.iov_len = sizeof(frame);
		msg.msg_namelen = sizeof(addr);
		msg.msg_controllen = sizeof(ctrlmsg);
		msg.msg_flags = 0;

		struct timespec timeout;
		timeout.tv_sec = 0;
		timeout.tv_nsec = 1000;

		recvmsg(baseSocket, &msg, 0);//MSG_DONTWAIT);

		for (cmsg = CMSG_FIRSTHDR(&msg);
			 cmsg && (cmsg->cmsg_level == SOL_SOCKET);
			 cmsg = CMSG_NXTHDR(&msg,cmsg)) {
		}


		//idx = idx2dindex(addr.can_ifindex, &addr);


		for(i = 0; i < frame.can_dlc; ++i)
		{
			cMsg.data[i] = frame.data[i];
		}
		cMsg.dlc = frame.can_dlc;
		cMsg.ext = true;
		cMsg.id = frame.can_id;

		//printf("Received msg with id %lu\n", cMsg.id);
	}
	return cMsg;
}

BlGenericMessage receiveGenericMessage()
{
	CanMessage cMsg = receiveMessage();
	BlGenericMessage msg;

	msg.FOF = ((cMsg.id>>28)&0x1);
	msg.commandId = (cMsg.id>>12)&0xFF;
	msg.targetDeviceId = (cMsg.id>>20)&0xFF;
	msg.length = cMsg.dlc;
	int i;
	for(i = 0; i < msg.length; ++i)
	{
		msg.data[i] = cMsg.data[i];
	}

	if(__VERBOSE)
		if(msg.commandId == STATUS_ID)
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

	return msg;
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
