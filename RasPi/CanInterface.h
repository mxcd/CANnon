/*
 * CanInterface.h
 *
 *  Created on: Mar 16, 2017
 *      Author: max
 */

#ifndef CANINTERFACE_H_
#define CANINTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "lib.h"

#include "CanMessage.h"

class CanInterface
{
public:
	CanInterface();
	~CanInterface();

	void sendMessage(CanMessage* msg);
	CanMessage* receiveMessage();
	bool available();
private:
	void initCanInterface();

	int baseSocket; /* can raw socket */
};



#endif /* CANINTERFACE_H_ */
