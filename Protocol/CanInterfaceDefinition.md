# CAN Interface Definition

This file defines the CAN bus communication between target and the CANnon bootloader.

###### Functionalities that need to be implementen in the bootloader:
* **[BL]** Running on startup, directly calling the user application
* **[CAN / BL]** Interrupting the current programm to start the bootloader
* **[BL]** Providing an EEPROM
* **[BL]** Storing a predefined target ID
* **[CAN / BL]** Responding to a CAN Ping broadcast
* **[CAN / BL]** Responding to a CAN Ping message
* **[CAN]** Setting the target ID
* **[CAN / BL]** Getting the current programm name and version
* **[CAN / BL]** Getting the current programm checksum
* **[CAN / BL]** Programming a new programm

##### Fancy future features
* Clone controller flash: Read flash from one device and write it to another

###### Required CAN messages:
* Broadcast Ping (a ping call everyone responds to)
* Single Ping (a ping call with an ID -> only the target with the id responds)
* Get program version
* Get program name
* Get EEPROM content
* Set EEPROM content
* Interrupt user application
* Send new program
* Get CRC of Flash

### CAN ID composition

We will use CAN extended IDs to communicate between **CANnon** and **target device**

The 29bit extended identifier is split up into multiple sections to enable masking and filtering of messages:

* **[29-25]:** CANnon message id
* **[24-13]:** Target Device id
* **[12-5]:** CRC of Data field (used when transmitting data)

#### CANnon message IDs
|MSG ID |Message Name             |Message Content              |
|-------|-------------------------|-----------------------------|
| 0x00  |Single ping request      |requester id                 |
| 0x01  |Broadcast ping request   |requester id                 |
| 0x02  |Ping response            |requester id / responder id  |
| 0x03  |Get program version      |requester id                 |
| 0x04  |Send programm version    |requester id / responder id / version |
| 0x05  |Get program name         |requester id                 |
| 0x06  |Send program name        |requester id / responder id / 1B packet id / 4B string


### target
#### CRC
* CRC in software
* CRC in hardware (STM32)
* CRC in hardware with DMA (STM32)
