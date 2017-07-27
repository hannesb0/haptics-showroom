//==============================================================================
/*
Filename:	MySerial2Arduino.h
Project:	Haptics Showroom
Authors:	Naina Dhingra, Ke Xu, Hannes Bohnengel
Revision:	0.2
Remarks:	These files are tracked with git and are available on the github
repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

#include "MySerial.h"


#define BAUDRATE 115200
const char* SERIAL_COM_PORT = "\\\\.\\COM4";




MySerial* mSerial;


void InitSerial2Arduino()
{
	mSerial = new MySerial((char*)SERIAL_COM_PORT);    // adjust as needed
	if (mSerial->IsConnected())
		printf("We're connected.\n");
	//char incomingData[256] = "";
}

void sendHot(bool hot) {
	// ####### TESTING #########
	//const int sizeInputBuffer = 50;
	//char inputBuffer[sizeInputBuffer] = { 0 };
	// ####### TESTING #########

	char buffer[] = "HOT#X\n";
	if (hot)
	{
		buffer[4] = '1';
	}
	else
	{
		buffer[4] = '0';
	}
	mSerial->WriteData(buffer, 6);

	// ####### TESTING #########
	//mSerial->ReadData(inputBuffer, sizeInputBuffer);
	//cout << inputBuffer;
}

void sendCold(bool cold) {
	char buffer[] = "COLD#X\n";
	if (cold)
	{
		buffer[5] = '1';
	}
	else
	{
		buffer[5] = '0';
	}
	mSerial->WriteData(buffer, 7);
}
