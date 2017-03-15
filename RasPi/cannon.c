/*
 * cannon.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: max
 */

#include "cannon.h"
#include "CanInterface.h"
#include "CanMessage.h"

int main(int argc, char **argv)
{
	CanMessage msg;
	msg.id = 42;
	msg.dlc = 4;

	int i;
	for(i = 0; i < 4; ++i)
	{
		msg.data[i] = i*2;
	}
	initCanInterface();
	sendMessage(&msg);
}
