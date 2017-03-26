/*
 * CanInterface.cpp
 *
 *  Created on: Mar 16, 2017
 *      Author: max
 */

#include "CanMessage.h"
#include "CanInterface.h"
#include <stdbool.h>

void initCanInterface()
{
	struct sockaddr_can addr;
	struct ifreq ifr;

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

	setsockopt(baseSocket, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	if (bind(baseSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
	}
}

bool available()
{
	return false;
}

CanMessage receiveMessage()
{
	CanMessage msg;
	return msg;
}

void sendMessage(CanMessage* msg)
{
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
