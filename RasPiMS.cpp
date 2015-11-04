#include <wiringPi.h>
#include <wiringSerial.h>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <cstring>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include "RasPiMS.hpp"

#include <iostream>

const int STX = 0x41;

using namespace std;
using namespace RPMS;

const int SEND_DATA_NUM = 7;
int MaxMotorPower = 200;

bool MotorSerial::wiringPiSetupGpioFlag = false;

MotorSerial::MotorSerial(int rede, int timeout, const char* devFileName, int bRate) {
	init(rede, timeout, devFileName, bRate);
}

MotorSerial::MotorSerial() {
	init(4, 10, "/dev/ttyAMA0", 115200);
}

void MotorSerial::init(int rede, int timeout, const char* devFileName, int bRate) {
	sumCheckSuccess = false;
	recentReceiveData = 0;
	threadLoopFlag = false;
	this->serialFileName = (char*)devFileName;
	this->bRate = bRate;
	this->timeOut = timeout;
	this->redePin = rede;
}

void MotorSerial::init() {
	this->serialFile = serialOpen(serialFileName, bRate);
	if (serialFile < 0) {
		throw runtime_error("SerialOpenError");
	}
	if (!wiringPiSetupGpioFlag) {
		if (wiringPiSetupGpio() < 0) {
			serialClose(serialFile);
			throw runtime_error("WiringPiSetupError");
		}
		wiringPiSetupGpioFlag = true;
	}
	pinMode(this->redePin, OUTPUT);
}

void MotorSerial::setTimeOut(int timeout) {
	timeOut = timeout;
}

short MotorSerial::sending(unsigned char id, unsigned char cmd, short data) {
	unsigned short uData = (unsigned short)data;
	unsigned char sendArray[SEND_DATA_NUM] = {0xFF, STX, id, cmd, (unsigned char)(uData % 0x100), (unsigned char)(uData / 0x100), (unsigned char)((id + cmd + uData % 0x100 + uData / 0x100) % 0x100)};
	lock_guard<mutex> lock(mtx);

	digitalWrite(this->redePin, 1);
	for (int i = 0; i < SEND_DATA_NUM; ++i) {
		serialPutchar(serialFile, sendArray[i]);
		delayMicroseconds(90);
	}
	digitalWrite(this->redePin, 0);

	bool stxFlag = false;
	char receiveArray[5] = {};
	int i = 0;

	auto startTime = chrono::system_clock::now();
	sumCheckSuccess = false;
	while(chrono::time_point<chrono::system_clock>(startTime + chrono::milliseconds(timeOut)) >= chrono::system_clock::now() && !sumCheckSuccess) {
		while(serialDataAvail(serialFile) > 0 ) {
			char gotData = serialGetchar(serialFile);
			if (gotData == STX && !stxFlag) {
				stxFlag = true;
				continue;
			}
			if (stxFlag) {
				receiveArray[i++] = gotData;
			}
			if (i > 4) {
				unsigned char sum = 0;
				for (int j = 0; j < 4; ++j)
					sum += receiveArray[j];
				if (sum == receiveArray[4]) {
					sumCheckSuccess = true;
					break;
				}
			}
		}
	}
	if (serialDataAvail(serialFile) < 0) {
		throw runtime_error("SerialComError");
	}
	recentReceiveData = receiveArray[2] + receiveArray[3] * 0x100;
	return recentReceiveData;
}

void MotorSerial::sendingLoop(void) {
	while (!sendDataQueue.empty()) {
		threadLoopFlag = true;
		SendDataFormat sendData = sendDataQueue.front();
		sendDataQueue.pop();
		sending(sendData.id, sendData.cmd, sendData.argData);
	}
	threadLoopFlag = false;
}

short MotorSerial::send(unsigned char id, unsigned char cmd, short data, bool asyncFlag) {
	if (asyncFlag) {
		SendDataFormat sendData = {id, cmd, data};
		sendDataQueue.push(sendData);
		if (!threadLoopFlag ) {
			if (sendThread.joinable())
				sendThread.join();
			sendThread = thread([&]{ sendingLoop(); });
			sched_param sch_params;
			sch_params.sched_priority = 1;
			if (pthread_setschedparam(sendThread.native_handle(), SCHED_RR, &sch_params)) {
				cerr << "Failed to set Thread scheduling :" << strerror(errno) << endl;
			}
		}
		return 0;
	}
	return sending(id, cmd, data);
}

short MotorSerial::send(SendDataFormat sendData, bool asyncFlag) {
	return send(sendData.id, sendData.cmd, sendData.argData, asyncFlag);
}

MotorSerial::~MotorSerial() {
	if (sendThread.joinable())
		sendThread.join();
	serialClose(this->serialFile);
}

Motor::Motor() {
	initFlag = false;
}

Motor::Motor(unsigned char id, unsigned char mNum, double magni, MotorSerial *ms, short maxPower) {
	this->id = id;
	this->mNum = mNum;
	this->magni = magni;
	this->ms = ms;
	this->maxPower = maxPower;
	initFlag = true;
}

Motor::Motor(MotorDataFormat MotorData, MotorSerial *ms, short maxPower) {
	this->id = MotorData.id;
	this->mNum = MotorData.mNum;
	this->magni = MotorData.magni;
	this->ms = ms;
	this->maxPower = maxPower;
	initFlag = true;
}

short Motor::changeMaxPower(short maxPower) {
	return (this-> maxPower = maxPower);
}

bool Motor::spin(short motorPower, bool asyncFlag) {
	if (!initFlag)
		return false;
	motorPower = (short)(magni * motorPower);
	if (motorPower > maxPower)
		motorPower = maxPower;
	if (motorPower < -maxPower)
		motorPower = -maxPower;
	return motorPower == ms->send(id, 2 + mNum, motorPower, asyncFlag);
}

Motor::~Motor() {
	spin(0);
}

int loadMotorSetting(char* FileName, MotorDataFormat* MotorDatas, int NumMotors) {	
	// 現在のパスを取得
	char buf[512] = {};
	readlink("/proc/self/exe", buf, sizeof(buf) - 1); 	// 実行ファイルのパスを取得
	string path(buf);
	path += FileName;
	// モーターの設定を読み込む
	ifstream settingFile;
	settingFile.open(path);
	if (!settingFile)
		return -1;
	for (int i = 0;(i < NumMotors) && (!settingFile.eof()); ++i) {
		for (int j = 0;j < 3 && (!settingFile.eof());++j) {
			string str;
			char* ends;
			settingFile >> str;
			if ((str.c_str()[0] >= '0' && str.c_str()[0] <= '9') || (j == 2 && str.c_str()[0] == '-')) {
				switch(j) {
				case 0:
					MotorDatas[i].id = atoi(str.c_str());
					break;
				case 1:
					MotorDatas[i].mNum = atoi(str.c_str());
					break;
				case 2:
					MotorDatas[i].magni = strtod(str.c_str(), &ends);
					break;
				}
			}
		}
	}
	return 0;
}
