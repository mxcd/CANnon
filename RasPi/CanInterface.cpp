/*
 * CanInterface.cpp
 *
 *  Created on: Mar 16, 2017
 *      Author: max
 */

#include "CanInterface.h"

CanInterface::CanInterface()
{
	baseSocket = 0;
}

CanInterface::~CanInterface()
{

}

void CanInterface::initCanInterface()
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

bool CanInterface::available()
{
	return false;
}

CanMessage* CanInterface::receiveMessage()
{
	CanMessage* msg = new CanMessage();
	return msg;
}

void CanInterface::sendMessage(CanMessage* msg)
{
	int nbytes;
	struct can_frame frame;
	frame.can_dlc = msg->getDlc();
	frame.can_id = msg->getId();
	for(int i = 0; i < msg->getDlc(); ++i)
	{
		frame.data[i] = msg->getData()[i];
	}

	if ((nbytes = write(baseSocket, &frame, sizeof(frame))) != sizeof(frame))
	{
		perror("write");
	}
}
