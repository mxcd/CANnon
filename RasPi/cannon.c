/*
 * cannon.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: max
 */

#include "config.h"
#include "cannon.h"
#include "CanInterface.h"
#include "CanMessage.h"

int main(int argc, char **argv)
{
	initCanInterface();
	if(argc < 2)
	{
		fprintf(stderr, "got no arg\n");
		// print help
		fprintf(stderr, "Please use the help\n");
	}
	else if(argc == 2)
	{
		fprintf(stderr, "Got one args\n");
		if(strcmp("ping",argv[1]) == 0)
		{
			fprintf(stderr, "Doing broadcast ping...\n");
			doBroadcastPing();
		}
	}
}

void doBroadcastPing()
{
	CanMessage msg;

	msg.id = 0 << 28;
	msg.id += BROADCAST_PING_ID << 12;
	msg.data[0] = CANNON_DEVICE_ID;
	msg.ext = 1;
	printf("Sending msg with CAN id %lu\n", msg.id);
	sendMessage(&msg);
}
