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
	CanMessage* msg = new CanMessage();
	msg->setId(42);
	msg->setDlc(4);
	msg->setData(new char[4]{2,4,5,8});

	CanInterface* can = new CanInterface();
	can->sendMessage(msg);

	delete(can);
}
