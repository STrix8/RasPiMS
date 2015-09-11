#include <wiringPi.h>
#include <wiringSerial.h>
#include <thread>
#include <chrono>
#include "RasPiMS.hpp"

#include <iostream>

const int STX = 0x41;

using namespace std;
using namespace RPMS;

const int SEND_DATA_NUM = 7;

const char *RPMS::SerialPortOpenError = "Cannot open serialport.";
const char *RPMS::WiringPiSetupError = "WiringPi Setup Error.";
const char *RPMS::SerialError = "Serial Error.";

bool MotorSerial::nowSendingFlag = false;
int MotorSerial::timeOut = 0;
int MotorSerial::serialFile = 0;
char *MotorSerial::serialFileName;
int MotorSerial::bRate = 0;
thread MotorSerial::sendThread;

MotorSerial::MotorSerial(int rede, int timeout, const char *devFileName, int bRate) {
	sumCheckSuccess = false;
	serialReceiveSuccess = false;
	recentReceiveData = 0;
	this->serialFileName = (char*)devFileName;
	this->bRate = bRate;
	this->timeOut = timeout;
	redePin = rede;
}

MotorSerial::MotorSerial() {
	MotorSerial(4);
}

void MotorSerial::init() {
	this->serialFile = serialOpen(serialFileName, bRate);
	if (serialFile < 0) {
		throw SerialPortOpenError;
	}
	if (wiringPiSetupGpio() < 0) {
		serialClose(serialFile);
		throw WiringPiSetupError;
	}
	pinMode(redePin, OUTPUT);
}

short MotorSerial::sending(unsigned char id, unsigned char cmd, short data){
	unsigned short uData = (unsigned short)data;
	unsigned char sendArray[SEND_DATA_NUM] = {0xFF, STX, id, cmd, (unsigned char)(uData % 0x100), (unsigned char)(uData / 0x100), (unsigned char)((id + cmd + uData % 0x100 + uData / 0x100) % 0x100)};
	while (nowSendingFlag);
	nowSendingFlag = true;
	
	digitalWrite(redePin, 1);
	for (int i = 0; i < SEND_DATA_NUM; ++i)
		serialPutchar(serialFile, sendArray[i]);
	digitalWrite(redePin, 0);
	
	bool stxFlag = false;
	char receiveArray[5] = {};
	int i = 0;

	auto startTime = chrono::system_clock::now();
	bool timeOutFlag = false;
/*
	while ((serialDataAvail(serialFile) < 7) && !timeOutFlag)
		timeOutFlag = chrono::time_point<chrono::system_clock>(startTime + chrono::milliseconds(timeOut)) < chrono::system_clock::now();
	if (timeOutFlag) {
		serialReceiveSuccess = false;
	} else {
		serialReceiveSuccess = true;
  */
  while(chrono::time_point<chrono::system_clock>(startTime + chrono::milliseconds(timeOut)) >= chrono::system_clock::now()) {
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
				if (sum == receiveArray[4]) {
					sumCheckSuccess = true;
					break;
				} else
					sumCheckSuccess = false; 
			}
		}
	}
	if (serialDataAvail(serialFile) < 0) {
		throw SerialError;
	}
	nowSendingFlag = false;
	recentReceiveData = receiveArray[2] + receiveArray[3] * 0x100;
	return recentReceiveData;
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
	if (sendThread.joinable())
		sendThread.join();
	serialClose(this->serialFile);
}
