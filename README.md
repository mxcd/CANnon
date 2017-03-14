# CANnon
#### A versatile CAN bootloader for the STM32 and friends

##### Why would anyone name a bootloader CANnon?
Well, there are several reasons for that:
* First off, there is the historical approach: a bootloader (or bootstrap loader) is named after the saying "to pull oneself up by one's bootstraps". Then there was this other guy named Baron Munchausen, who famously pulled himself out of the swamp by his pigtail. Munchausen was also good at riding cannon balls, so there you have it.
* Secondly, it is supposed to load the program on the micro controller via CAN. CANnon... you get it
* Thirdly, we are shooting some .bin files at the controller, so that seemed legit.

##### Modules

The CANnon bootloader consists of two modules:  
* The CANnon that shoots the CANonball (.bin file) at the target
* The target code that accepts what the CANon shoots at it
