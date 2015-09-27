#pragma once

#include <fstream>
#include <thread>
#include <queue>
#include <mutex>

namespace RPMS {
	typedef struct {
		unsigned char id;
		unsigned char cmd;
		short argData;
	}sendDataFormat;
	class MotorSerial {
	public:
		MotorSerial(int, int timeout = 10, const char *devFileName = "/dev/ttyAMA0", int bRate = 115200);
		MotorSerial();
		void init();
		void setTimeOut(int);
		short send(unsigned char, unsigned char, short, bool asyncFlag = false);
		short send(sendDataFormat, bool asyncFlag = false);
		short sending(unsigned char, unsigned char, short);
		virtual ~MotorSerial();
		bool sumCheckSuccess;
		short recentReceiveData;
//		bool serialReceiveSuccess;
	private:
		void init(int, int, const char*, int);  
		void sendingLoop(void);
		char *serialFileName;
		bool nowSendingFlag;
		bool threadLoopFlag;
		int timeOut;
		int serialFile;
		int bRate;
		int redePin;
		std::thread sendThread;
		std::queue<sendDataFormat> sendDataQueue;
		std::mutex mtx;
	};
}
