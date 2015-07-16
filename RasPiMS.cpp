#include <wiringPi.h>
#include <wiringSerial.h>
#include <thread>
#include "RasPiMS.hpp"

#include <iostream>

const int STX = 0x41;

using namespace std;
using namespace RPMS;

const char *RPMS::SerialPortOpenError = "Cannot open serialport.";
const char *RPMS::WiringPiSetupError = "WiringPi Setup Error.";
const char *RPMS::SerialError = "Serial Error.";

bool MotorSerial::nowSendingFlag = false;
double MotorSerial::timeOut = 0.0;
int MotorSerial::serialFile = 0;
thread MotorSerial::sendThread;

MotorSerial::MotorSerial(int rede, double timeout, const char *devFileName, int bRate) {
	this->serialFile = serialOpen(devFileName, bRate);
	if (serialFile < 0) {
		throw SerialPortOpenError;
	}
	this->timeOut = timeout;
	redePin = rede;
	if (wiringPiSetupGpio() < 0) {
		serialClose(serialFile);
		throw WiringPiSetupError;
	}
	pinMode(redePin, OUTPUT);
}

MotorSerial::MotorSerial() {
	MotorSerial(4);
}

short MotorSerial::sending(unsigned char id, unsigned char cmd, short data){
	unsigned char sendArray[7] = {0xFF, STX, id, cmd, (unsigned char)(data % 0x100), (unsigned char)(data / 0x100), (unsigned char)((id + cmd + data % 0x100 + data / 0x100) % 0x100)};
	while (nowSendingFlag);
	nowSendingFlag = true;
	
	digitalWrite(redePin, 1);	
	serialPuts(serialFile, (const char*)sendArray);
	digitalWrite(redePin, 0);
	
	bool stxFlag = false;
	char receiveArray[5] = {};
	int i = 0;

	while(serialDataAvail(serialFile) > 0) {
		char gotData = serialGetchar(serialFile);
		if (gotData == STX && !stxFlag) {
			stxFlag = true;
			continue;
		}
		if (stxFlag) {
			receiveArray[i++] = gotData;
		}
		if (i > 4) {
			int sum = 0;
			for (int j = 0; j < 4; ++j) 
				sum = receiveArray[j];
			if (sum == receiveArray[4])
				break;
		}
	}
	if (serialDataAvail(serialFile) < 0) {
		throw SerialError;
	}
	nowSendingFlag = false;
	return receiveArray[2] + receiveArray[3] * 0x100;
}

short MotorSerial::send(unsigned char id, unsigned char cmd, short data, bool multiThread) {
	if (multiThread) {
		if (sendThread.joinable())	
			sendThread.join();
		sendThread = thread([&]{ sending(id, cmd, data); });	
		return 0;
	}
	return sending(id, cmd, data);
}

short MotorSerial::send(sendDataFormat sendData, bool multiThread) {
	return send(sendData.id, sendData.cmd, sendData.argData, multiThread);
}

MotorSerial::~MotorSerial() {
	serialClose(this->serialFile);
}
