/*
 * CanInterface.h
 *
 *  Created on: Mar 16, 2017
 *      Author: max
 */

#ifndef CANINTERFACE_H_
#define CANINTERFACE_H_

#include <stdio.h>
#include <stdbool.h>
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

#define MAXANI 4
#define MAXSOCK 16    /* max. number of CAN interfaces given on the cmdline */
#define MAXIFNAMES 30 /* size of receive name index to omit ioctls */
#define MAXCOL 6      /* number of different colors for colorized output */
#define ANYDEV "any"  /* name of interface to receive from any CAN interface */
#define ANL "\r\n"    /* newline in ASC mode */


void sendGenericMessage(BlGenericMessage* msg);
void sendMessage(CanMessage* msg);
CanMessage receiveMessage();
bool canAvailable();
void initCanInterface();
BlGenericMessage receiveGenericMessage();
//int idx2dindex(int ifidx, int socket);
int baseSocket; /* can raw socket */
struct iovec iov;
struct msghdr msg;
struct can_frame frame;
char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
struct sockaddr_can addr;
struct ifreq ifr;
fd_set rdfs;
static int  dindex[MAXIFNAMES];

#endif /* CANINTERFACE_H_ */
