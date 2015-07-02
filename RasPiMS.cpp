#include <wiringPi.h>
#include <wiringSerial.h>
#include <thread>
#include "RasPiMS.hpp"

const int STX = STX;

using namespace std;
using namespace RPMS;

bool nowSendingFlag = false;
double timeOut = 0.0;
int serialFile = NULL;
thread sendThread = nullptr;

MoterSerial::MoterSerial(int rede, double timeout, char *devFileName, int bRate) {
	serialFile = serialOpen(devFileName, bRate);
	if (serialFile < 0) {
		puts("Cannot open serialport.");
		return;
	}
	timeOut = timeout;
	redePin = rede;
	if (wiringPiSetupGpio() < 0) {
		puts("WiringPi Setup Error");
		return;
	}
	pinMode(redePin, OUTPUT);
}

MoterSerial::MoterSerial() {
	MoterSerial(4);
}

short MoterSerial::sending(unsigned char id, unsigned char cmd, short data){
	char *sendArray = [0xFF, STX, id, cmd, data % 0x100, data / 0x100, (id + cmd + data % 0x100 + data / 0x100) % 0x100];
	while (nowSendingFlag);
	nowSendingFlag = true;
	
	digitalWrite(redePin, 1);	
	serialPuts(serialFile, sendArray);
	digitalWrite(redePin, 0);

	bool stxFlag = false;
	char receiveArray[5] = {};
	int i = 0;
	while(serialDataAvail(serialFile)) {
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
	nowSendingFlag = false;
	return receiveArray[2] + receiveArray[3] * 0x100;
}

short MoterSerial::send(unsigned char id, unsigned char cmd, short data, bool multiThread) {
	if (multiThread) {
		if (sendThread.joinable())	
			sendThread.join();
		sendThread = thread(sending, id, cmd, data);	
		return 0;
	}
	return sending(id, cmd, data);
}

short MoterSerial::send(sendDataFormat sendData, bool multiThread) {
	return send(sendData.id, sendData.cmd, sendData.data, multiThread);
}
