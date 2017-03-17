/*
 * CanMessage.h
 *
 *  Created on: Mar 16, 2017
 *      Author: max
 */

#ifndef CANMESSAGE_H_
#define CANMESSAGE_H_

class CanMessage
{
public:
	CanMessage()
	{
		this->dlc = 0;
		this->id = 0;

		this->data = new char[8]{0,0,0,0,0,0,0,0};
	};
	~CanMessage(){};

	void setDlc(char dlc){ this->dlc = dlc; };
	void setId(long id) { this->id = id; };
	void setData(char data[8]) { this->data = data; };

	char getDlc() { return dlc; };
	long getId() { return id; };
	char* getData() { return data; };
private:
	char dlc;
	long id;
	char* data;
};

#endif /* CANMESSAGE_H_ */
