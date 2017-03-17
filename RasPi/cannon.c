/*
 * cannon.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: max
 */

#include "config.h"
#include "cannon.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "CanInterface.h"
#include "CanMessage.h"
#include <time.h>

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
		printf("Got one args\n");
		if(strcmp("ping",argv[1]) == 0)
		{
			printf("Doing broadcast ping...\n");
			doBroadcastPing();
		}
	}
	else if(argc == 3)
	{
		if(strcmp("interrupt", argv[1]) == 0)
                {
			sendSignalMessage(strtol(argv[2], NULL, 0), INTERRUPT_ID);
			printf("Sent interrupt\n");
                }
	}
	else if(argc == 4)
	{
		printf("Got three args\n");
		if(strcmp("flash", argv[1]) ==0)
		{
			printf("Doing flash\n");
			doFlash(argv[2], argv[3]);
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

void doFlash(char* file, char* device)
{
	int deviceId = strtol(device, NULL, 0);
	printf("Flashing file %s on device %i\n", file, deviceId);

	int size = getFileSize(file);

	sendSignalMessage(deviceId, INTERRUPT_ID);
	char* binArray = readFile(file);

	printf("Waiting for flash to be erased");
	sendSignalMessage(deviceId, INIT_FLASH_ID);
	int i;
	for(i = 0; i < 10; ++i)
	{
		waitFor(1);
		printf(".");
	}
	printf("done\n");
	usleep(50000);
	startFlashing(deviceId, 64, size);
	usleep(10000);


	for(i = 0; i < size/8; ++i)
	{
		usleep(500);
		int j;
		char pack[8];
		for(j = 0; j < 8; ++j)
		{
			pack[j] = binArray[i*8 + j];
		}
		sendFlashPack(deviceId, i, pack, 8);

		if(i != 0 && i%64 == 63)
		{
			printf("Sprint %i\n", i/64);
			usleep(500);
			sendSignalMessage(deviceId, ACK_REQUEST_ID);
		}
	}

	usleep(500);
	int remainder = size%8;
	if(remainder != 0)
	{
		int base = size-((size/8)*8);
		int i;
		char pack[8];
		for(i = 0; i < remainder; ++i)
		{
			pack[i] = binArray[base + i];
		}
		sendFlashPack(deviceId, size/8, pack, remainder);
	}

	printf("loading done");

	sendSignalMessage(deviceId, END_FLASH_ID);
	sendSignalMessage(deviceId, EXIT_FLASH_ID);
	sendSignalMessage(deviceId, START_APP_ID);
}

void sendSignalMessage(int deviceId, char command)
{
	BlGenericMessage msg;

        msg.FOF = 0;
        msg.targetDeviceId = deviceId;
        msg.flashPackId = 0;
        msg.commandId = command;
        msg.length = 0;
        sendGenericMessage(&msg);
}

void startFlashing(int deviceId, int packsPerSprint, int size)
{
	BlGenericMessage msg;

        msg.FOF = 0;
        msg.targetDeviceId = deviceId;
        msg.flashPackId = 0;
        msg.commandId = START_FLASH_ID;
        msg.length = 4;
	msg.data[0] = size&0xFF;
	msg.data[1] = (size>>8)&0xFF;
	msg.data[2] = (size>>16)&0xFF;
	msg.data[3] = packsPerSprint&0xFF;
        sendGenericMessage(&msg);

}

void sendFlashPack(int deviceId, int packId, char* data, int len)
{
	BlGenericMessage msg;

        msg.FOF = 1;
        msg.targetDeviceId = deviceId;
        msg.flashPackId = packId;
        msg.commandId = 0;
        msg.length = 8;
        int i;
	for(i = 0; i < 8; ++i)
	{
		if(i < len)
		{
			msg.data[i] = data[i];
		}
		else
		{
			msg.data[i] = 0;
		}
	}
        sendGenericMessage(&msg);
}

char* readFile(char* file)
{
	int size;
	size = getFileSize(file);
	FILE* fp;
	fp = fopen(file, "rb");
	char* data = (char*) malloc(sizeof(char)*size);
	fread(data, 1, size, fp);
	int i;
	printf("Data read!\n\n");

	fclose(fp);
	return data;
}

int getFileSize(char* file)
{
	int size;
        FILE* fp;
        fp = fopen(file, "rb");
        fseek(fp, 0L, SEEK_END);
        size = ftell(fp);

        printf("File size: %i\n", size);

        fclose(fp);

	return size;
}

void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}
