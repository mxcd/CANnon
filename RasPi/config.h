/*
 * config.h
 *
 *  Created on: Mar 18, 2017
 *      Author: max
 */

#ifndef CONFIG_H_
#define CONFIG_H_


/**
 * Command ID defines start
 */
#define SINGLE_PING_ID		0x00 // OUTGOING   -->
#define BROADCAST_PING_ID	0x01 // OUTGOING   -->
#define PING_RESPONSE_ID	0x02 // INCOMING <--
#define INTERRUPT_ID		0x03 // OUTGOING   -->
#define GET_VERSION_ID		0x04 // OUTGOING   -->
#define SEND_VERSION_ID		0x05 // INCOMING <--
#define GET_NAME_ID			0x06 // OUTGOING   -->
#define SEND_NAME_ID		0x07 // INCOMING <--
#define INIT_FLASH_ID		0x08 // OUTGOING   -->
#define ACK_REQUEST_ID		0x09 // OUTGOING   -->
#define ACK_ID				0x0A // INCOMING <--
#define NACK_ID				0x0B // INCOMING <--
#define EXIT_FLASH_ID		0x0C // OUTGOING   -->
#define GET_CRC_ID			0x0D // OUTGOING   -->
#define SEND_CRC_ID			0x0E // INCOMING <--
#define START_FLASH_ID		0x0F // OUTGOING   -->
#define END_FLASH_ID		0x10 // OUTGOING   -->
#define STATUS_ID			0x11 // INCOMING <--
#define GET_CHIP_ID			0x12 // OUTGOING   -->
#define SEND_CHIP_ID		0x13 // INCOMING <--
#define START_APP_ID		0xFF // OUTGOING   -->
/**
 * Command ID defines end
 */

/**
 * Error Codes
 */
#define ERRCODE_OK 						0U
#define ERRCODE_NO_FLASH_PROCESS 		1U
#define ERRCODE_NOT_IN_FLASH_MODE 		2U
#define ERRCODE_NOT_IN_BOOT_MENU		3U
#define ERRCODE_ALREADY_FLASHING		4U
#define STATUS_IN_BOOT_MENU				5U
#define STATUS_START_ERASE				6U
#define STATUS_ERASE_FINISHED			7U
#define STATUS_FLASH_START				8U
#define STATUS_FLASH_DONE				9U
#define STATUS_STARTING_APP				10U
/*
 * Error Codes end
 */

#define CANNON_DEVICE_ID 42U
#define __VERBOSE 0

#endif /* CONFIG_H_ */
