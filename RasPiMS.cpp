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

MoterSerial::MoterSerial(int rede, double timeout, char *devFileName, int bRate) {
	serialFile = serialOpen(devFileName, bRate);
	if (serialFile < 0) {
		puts("Cannot open serialport.");
	}
	timeOut = timeout;
	redePin = rede;
}

MoterSerial::MoterSerial() {
	MoterSerial(4);
}

short MoterSerial::send(char id, char cmd, short data) {
	char *sendArray = [0xFF, STX, id, cmd, data % 0x100, data / 0x100, (id + cmd + data % 0x100 + data / 0x100) % 0x100];
	while (nowSendingFlag);
	nowSendingFlag = true;
	serialPuts(serialFile, sendArray);

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

short MoterSerial::send(sendDataFormat sendData) {
	return send(sendData.id, sendData.cmd, sendData.data);
}
