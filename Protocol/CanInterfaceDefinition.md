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
* Update Bootloader with bootloaded application

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
* Do CRC self check

### Message ID and Target ID
#### Target Device IDs
The IDs of the target devices will be stored in one byte. This limits the number of targets on a single bus to 256.  
The target device id will always be part of the CAN message id.

#### CAN ID composition
We will use CAN extended IDs to communicate between **CANnon** and **target device**

The 29bit extended identifier is split up into multiple sections to enable masking and filtering of messages:

* **[28]:** Flash Operation Flag (FOF)  
* **[27-20]:** Target Device ID  
* **If FOF == 1:**
 * **[19-0]:** Flash Pack ID  
* **If FOF == 0:**
 * **[19-12]:** Command ID

The target device ID is immutable per communication, meaning that the same target device id will be used not only for the request, but also for the response.

#### Command IDs
|CMD ID |Command Name             |DLC|Message Content              |
|-------|-------------------------|---|-----------------------------|
| 0x00  |Single ping request      |  1|requester id                 
| 0x01  |Broadcast ping request   |  1|requester id                 
| 0x02  |Ping response            |  2|requester id / responder id  
| 0x03  |Interrupt startup        |  0|
| 0x04  |Get program version      |  1|requester id                 
| 0x05  |Send programm version    |  8|requester id / responder id / version (48 LSB of commit id)
| 0x06  |Get program name         |  1|requester id                 
| 0x07  |Send program name        |  8|requester id / responder id / 1B packet id / 1B pack count / 4B string
| 0x08  |Init flashing process    |  8|Total Number of Bytes (20 Bit)
| 0x09  |ACK                      |  0|
| 0x0A  |NACK                     |  2|10 Bit Missing Packs

### target
#### CRC
* CRC in software
* CRC in hardware (STM32)
* CRC in hardware with DMA (STM32)

Store CRC in EEPROM
Check Flash on request


### User Stories
* [x]1: Bootloader project can be flashed on STM32
* [ ]2: Bootloader sends CAN message on startup
* [ ]3: Bootloader forwards to user application after time X
* [ ]4: Bootloader stops forwarding when CAN message is received
* [ ]5: Bootloader works with User Application in flash
* [ ]6: Bootloader accepts Flash Messages
* [ ]7: Bootloader stores transfered flash messages
* [ ]8: Bootloader calculates CRC checksum while flashing
* [ ]9: Bootloader sends back CRC checksum once done flashing
* [ ]more fancy stuff
