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
	struct can_filter rfilter[4];

	if(deviceId != 0)
	{
		rfilter[0].can_id = CAN_EFF_FLAG | deviceId << 20;
		rfilter[0].can_mask = CAN_EFF_FLAG | 0xFF << 20;
	}
	else
	{
		rfilter[0].can_id = CAN_EFF_FLAG;
		rfilter[0].can_mask = CAN_EFF_FLAG;
	}

	rfilter[1].can_id = 0x421;
	rfilter[1].can_mask = 0x7FF;

	rfilter[2].can_id = 0x424;
	rfilter[2].can_mask = 0x7FF;

	rfilter[3].can_id = 0x427;
	rfilter[3].can_mask = 0x7FF;

	setsockopt(baseSocket, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, 4*sizeof(struct can_filter));

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

int receiveMessage(CanMessage* cMsg)
{
	int i;
	int nbytes, count = 0;
	struct cmsghdr *cmsg;

	cMsg->dlc = 0;
	cMsg->id = 0;
	cMsg->ext = 0;
	for(i = 0; i < 8; ++i)
		cMsg->data[i] = 0;

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

		nbytes = recvmsg(baseSocket, &msg, MSG_DONTWAIT);
		if(nbytes > 0)
		{
			for (cmsg = CMSG_FIRSTHDR(&msg);
				cmsg && (cmsg->cmsg_level == SOL_SOCKET);
				cmsg = CMSG_NXTHDR(&msg,cmsg)) {
			}

			//idx = idx2dindex(addr.can_ifindex, &addr);
			/*
			printf("%x:", frame.can_id);
			for(i = 0; i < frame.can_dlc; ++i)
			{
				cMsg->data[i] = frame.data[i];
				printf("[%x]", frame.data[i]);
			}
			printf("\n");*/
			cMsg->dlc = frame.can_dlc;
			cMsg->ext = true;
			cMsg->id = frame.can_id;
		}
		//printf("Received msg with id %lu\n", cMsg.id);
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
	if(__VERBOSE)
	{
		printf("msg send");
		fflush(stdout);
	}

	CanMessage cMsg;
	genericToMsg(msg, &cMsg);
	sendMessage(&cMsg);
}

void sendMessage(CanMessage* msg)
{
	if(__VERBOSE)
	{
		printf("cMsg send");
		fflush(stdout);
	}

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

void clearInputBuffer()
{
	int nbytes;
	do
	{
		CanMessage msg;
		nbytes = receiveMessage(&msg);
	}
	while(nbytes > 0);
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

