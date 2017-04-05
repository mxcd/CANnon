/*
 * cannon.h
 *
 *  Created on: Mar 17, 2017
 *      Author: max
 */

#ifndef CANNON_H_
#define CANNON_H_

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

//int main(int argc, char **argv);
void doBroadcastPing();
void sendSignalMessage(int deviceId, char command);
void doFlash(char* deviceName);
char* readFile(char* file);
int getFileSize(char* file);
void resetDevice(DEVICE_CONFIG device);
void restoreDevicePower();
void waitFor (unsigned int secs);
void startFlashing(int deviceId, int packsPerSprint, int size);
void sendFlashPack(int deviceId, int packId, char* data, int len);
void waitForSignal(int deviceId, int statusFlag, int triggerMessage, int triggerRate);
DEVICE_CONFIG findDeviceByName(char* deviceName);
char* concat(const char *s1, const char *s2);

int lvs_f_on_states = 0;
int lvs_r1_on_states = 0;
int lvs_r2_on_states = 0;

#endif /* CANNON_H_ */
