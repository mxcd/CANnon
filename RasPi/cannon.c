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
		fprintf(stderr, "got no arg");
		// print help
		fprintf(stderr, "Please use the help");
	}
	else if(argc == 2)
	{
		fprintf(stderr, "Got one args");
		if(strcmp("ping",argv[1]) == 0)
		{
			fprintf(stderr, "Doing broadcast ping...");
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
	sendMessage(&msg);
}
