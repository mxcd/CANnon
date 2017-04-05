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
	initTargetDeviceConfig();

	if(argc < 2)
	{
		fprintf(stderr, "got no arg\n");
		// print help
		fprintf(stderr, "Please use the help\n");
	}
	else if(argc == 2)
	{
		initCanInterface(0);

		if(__VERBOSE)
			printf("Got one args\n");
		if(strcmp("ping",argv[1]) == 0)
		{
			printf("Doing broadcast ping");
			fflush(stdout);
			int startTime = clock();

			if(__VERBOSE)
				printf("Start time: %i\n", startTime);

			doBroadcastPing();
			while(clock()-startTime < 50000)
			{
				BlGenericMessage msg;

				int nbytes = receiveGenericMessage(&msg);
				if(nbytes > 0)
				{
					if(msg.commandId == PING_RESPONSE_ID)
					{
						int pingTime = clock() - startTime;
						printf("Found device ID %i after %i ms\n", msg.targetDeviceId, pingTime);
						fflush(stdout);
					}
				}
			}
		}
		else if(strcmp("peek",argv[1]) == 0)
		{
			printf("Listening on CAN bus\n");
			while(1)
			{
				BlGenericMessage msg;

				int nbytes = receiveGenericMessage(&msg);
				if(nbytes > 0)
				{
					printf("%i \t-> %i : %i[", clock(), msg.targetDeviceId, msg.FOF, msg.length);
					int i;
					for(i = 0; i < msg.length; ++i)
					{
						printf("%i ", msg.data[i]);
					}
					printf("]\n");
				}
				usleep(500);
			}
		}
	}
	else if(argc == 3)
	{
		if(__VERBOSE)
			printf("Got three args\n");

		if(strcmp("interrupt", argv[1]) == 0)
		{
			initCanInterface(0);
			sendSignalMessage(strtol(argv[2], NULL, 0), INTERRUPT_ID);
			printf("Sent interrupt\n");
		}
		else if(strcmp("flash", argv[1]) ==0)
		{
			doFlash(argv[2]);
		}
	}
}

void waitForSignal(int deviceId, int statusFlag, int triggerMessage, int triggerRate)
{
	bool nack = true;
	long counter = 0;

	if(__VERBOSE)
		printf("\nWait for signal\n");

	while(nack)
	{
		if(counter % 1000 == 0)
		{
			printf(".");
			fflush(stdout);
		}

		if(triggerMessage != 0)
		{
			if(counter % triggerRate == 0)
			{
				sendSignalMessage(deviceId, triggerMessage);
				printf("+");
			}
		}

		int nbytes;
		do
		{
			BlGenericMessage msg;
			nbytes = receiveGenericMessage(&msg);

			if(nbytes > 0)
			{
				//printf("Received msg: ID:%x TDI:%x\n", msg.commandId, msg.targetDeviceId);
				if(msg.targetDeviceId == deviceId && msg.commandId == STATUS_ID && msg.data[0] == statusFlag)
				{
					nack = false;
					printf(" ok\n");
					fflush(stdout);
					break;
				}
			}
		}
		while(nbytes > 0);
		usleep(1);
		++counter;
	}
}

void doBroadcastPing()
{
	BlGenericMessage msg;

	msg.FOF = 0;
	msg.commandId = BROADCAST_PING_ID;
	msg.data[0] = CANNON_DEVICE_ID;
	msg.flashPackId = 0;
	msg.length = 1;
	msg.targetDeviceId = 0;

	sendGenericMessage(&msg);
}

void doFlash(char* deviceName)
{
	int i, j;
	int packsPerSprint = 64;

	DEVICE_CONFIG device = findDeviceByName(deviceName);

	int deviceId = device.id;
	char* file = concat("/home/max/gt_carstate/", concat(device.name, ".bin"));
	printf("Flashing file %s on device %i\n", file, deviceId);

	initCanInterface(deviceId);

	int size = getFileSize(file);
	char* binArray = readFile(file);

	// Very verbose printing of complete bin file
	/*
	int counter = 0;
	for(i = 0; i < (size/16)+1; ++i)
	{
		printf("#%x\t", counter);
		for(j = 0; j < 4; ++j)
		{
			printf("[");
			int k;
			for(k = 0; k < 4; ++k)
			{
				if(counter < size)
				{
					printf("%x ", binArray[counter]);
					counter++;
				}
			}
			printf("]\t");
		}
		printf("\n");
	}
	*/

	printf("Resetting device to enter bootloader\n");
	fflush(stdout);

	resetDevice(device);

	printf("Waiting for interrupt to be confirmed");
	fflush(stdout);
	// Wait for InterruptConfirm

	clearInputBuffer();

	waitForSignal(deviceId, STATUS_IN_BOOT_MENU, INTERRUPT_ID, 500);

	usleep(500000);
	printf("Waiting for flash to be erased");
	fflush(stdout);

	sendSignalMessage(deviceId, INIT_FLASH_ID);

	waitForSignal(deviceId, STATUS_ERASE_FINISHED, 0, 0);

	usleep(50000);

	startFlashing(deviceId, packsPerSprint, size);
	usleep(50000);
	printf("Flashing");
	fflush(stdout);

	int sprintBasePack = 0;
	bool nack = true;

	for(i = 0; i < size/8; ++i)
	{
		usleep(500);

		char pack[8];
		for(j = 0; j < 8; ++j)
		{
			pack[j] = binArray[i*8 + j];
		}
		sendFlashPack(deviceId, i, pack, 8);

		if(i != 0 && i%packsPerSprint == packsPerSprint-1)
		{
			if(__VERBOSE)
				printf("Sprint %i\n", i/packsPerSprint);
			else
				printf(".");
			fflush(stdout);
			usleep(500);
			sendSignalMessage(deviceId, ACK_REQUEST_ID);

			nack = true;
			while(nack)
			{
				BlGenericMessage msg;
				int nbytes = receiveGenericMessage(&msg);
				if(nbytes > 0)
				{
					if(msg.targetDeviceId == deviceId && msg.commandId == ACK_ID)
					{
						nack = false;
						break;
					}
					else if(msg.targetDeviceId == deviceId && msg.commandId == NACK_ID)
					{
						printf("\nReceived NACK!\n");
						int k;
						long sprintFlags = 0;
						for(k = 0; k < msg.length; ++k)
						{
							sprintFlags += msg.data[k] << (k*8);
						}

						for(k = 0; k < packsPerSprint; ++k)
						{
							if(!((sprintFlags>>k)&0x1))
							{
								printf("0");
								//printf("\nResending pack %i\n", sprintBasePack + k);
								fflush(stdout);
								for(j = 0; j < 8; ++j)
								{
									pack[j] = binArray[sprintBasePack + k + j];
								}
								sendFlashPack(deviceId, sprintBasePack + k, pack, 8);
								usleep(500);
							}
							else
							{
								printf("1");
							}
						}
						sendSignalMessage(deviceId, ACK_REQUEST_ID);
						usleep(500);
					}
				}
				//usleep(100000);
			}
			sprintBasePack = i+1;
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
	// TODO: add ACK check on end
	printf(" done\n");

	usleep(500000);
	sendSignalMessage(deviceId, END_FLASH_ID);
	usleep(500);
	sendSignalMessage(deviceId, EXIT_FLASH_ID);
	usleep(500);

	clearInputBuffer();

	sendSignalMessage(deviceId, START_APP_ID);
	printf("starting user app");

	waitForSignal(deviceId, STATUS_STARTING_APP, 0, 0);

	restoreDevicePower();

	printf("\nThanks for traveling with air penguin!\n");
}

void resetDevice(DEVICE_CONFIG device)
{
	int lvs_can_id;

	printf("Resetting ");
	switch(device.LVS)
	{
	case LVS_F:
		printf("LVS_Front");
		lvs_can_id = LVS_F_CAN_ID;
		break;
	case LVS_R1:
		printf("LVS_Rear1");
		lvs_can_id = LVS_R1_CAN_ID;
		break;
	case LVS_R2:
		printf("LVS_Rear2");
		lvs_can_id = LVS_R2_CAN_ID;
		break;
	case LVS_BMS:
		printf("LVS_BMS");
		// TODO Ask Soeren for LVS_BMS ID
		break;
	}
	printf(" CH %i\n", device.CH);

	int startTime = clock();
	printf("saving power states...");
	while(clock()-startTime < 500000)
	{
		CanMessage msg;
		receiveMessage(&msg);

		if(msg.id == 0x421)
		{
			lvs_f_on_states = msg.data[0] + (msg.data[1]<<8);
		}
		else if(msg.id == 0x424)
		{
			lvs_r1_on_states = msg.data[0] + (msg.data[1]<<8);
		}
		else if(msg.id == 0x427)
		{
			lvs_r2_on_states = msg.data[0] + (msg.data[1]<<8);
		}
	}
	printf("done\n");

	//int dataOff = 1 << device.CH;
	int dataOff = 0xFFF;
	int dataOn = (1 << device.CH) | (1 << (16+device.CH));


	CanMessage msg;
	msg.ext = false;
	msg.dlc = 4;
	msg.id = LVS_F_CAN_ID;
	int i;
	for(i = 0; i < 4; ++i)
	{
		msg.data[i] = dataOff >> (i*8);
	}
	sendMessage(&msg);
	msg.id = LVS_R1_CAN_ID;
	sendMessage(&msg);
	msg.id = LVS_R2_CAN_ID;
	sendMessage(&msg);

	usleep(1000000);
	msg.id = lvs_can_id;
	for(i = 0; i < 4; ++i)
	{
		msg.data[i] = dataOn >> (i*8);
	}
	sendMessage(&msg);
}

void restoreDevicePower()
{
	printf("Restoring power...");
	CanMessage msg;
	msg.ext = false;
	msg.dlc = 4;
	msg.id = LVS_F_CAN_ID;
	msg.data[0] = 0xFF;
	msg.data[1] = 0x0F;
	msg.data[2] = lvs_f_on_states;
	msg.data[3] = (lvs_f_on_states >> 8)&0x0F;
	sendMessage(&msg);

	msg.id = LVS_R1_CAN_ID;
	msg.data[2] = lvs_r1_on_states;
	msg.data[3] = (lvs_r1_on_states >> 8)&0x0F;
	sendMessage(&msg);

	msg.id = LVS_R2_CAN_ID;
	msg.data[2] = lvs_r2_on_states;
	msg.data[3] = (lvs_r2_on_states >> 8)&0x0F;
	sendMessage(&msg);
	printf(" done\n");
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

	/*
	if(len != 8)
	{
		printf("\n\n%x\n%i\n", packId*8, len);
		int i = 0;
		for(;i < len; ++i)
		{
			printf(" [%x] ", data[i]);
		}
		printf("\n\n");
//		data[0] = 0x0;
//		data[1] = 0x24;
//		data[2] = 0xf4;
//		data[3] = 0x0;
	}
	*/
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
			msg.data[i] = 0xFF;
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

DEVICE_CONFIG findDeviceByName(char* deviceName)
{
	int i;
	for(i = 0; i < maxDeviceCount; ++i)
	{
		if(strcmp(blconf[i].name, deviceName) == 0)
		{
			return blconf[i];
		}
	}

	fprintf(stderr, "NO MATCHING CONFIG FOUND!!!");
	fflush(stdout);
	fflush(stderr);
	exit(-1);
}

void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
