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

// this has to be changed depending on what computer this program is executed
const char* SERIAL_COM_PORT = "\\\\.\\COM4";

MySerial* mSerial;

void InitSerial2Arduino()
{
	mSerial = new MySerial((char*)SERIAL_COM_PORT);    // adjust as needed
	if (mSerial->IsConnected())
		printf("Arduino connected!\n");
	//char incomingData[256] = "";
}

void sendHot(char temp)
{
	char buffer[] = "HOT#X\n";
	buffer[4] = temp;
	mSerial->WriteData(buffer, 6);
}

void sendCold(char temp) {
	char buffer[] = "COLD#X\n";
	buffer[5] = temp;
	mSerial->WriteData(buffer, 7);
}

void sendReset()
{
	char buffer[] = "RESET";
	mSerial->WriteData(buffer, 5);
}

void sendTemperature(int temperature)
{
	char intensity = '0';

	switch (temperature)
	{
	case 1: // very cold
		sendCold('2');
		break;
	case 2: // cold
		sendCold('1');
		break;
	case 3: // normal
		sendCold('0');
		break;
	case 4:	// hot
		sendHot('1');
		break;
	case 5: // very hot
		sendHot('2');
		break;
	default:
		sendReset();
		break;
	}
}