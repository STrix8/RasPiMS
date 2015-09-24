#pragma once

#include <fstream>
#include <thread>
#include <queue>

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
		static void setTimeOut(int);
		short send(unsigned char, unsigned char, short, bool asyncFlag = false);
		short send(sendDataFormat, bool asyncFlag = false);
		short sending(unsigned char, unsigned char, short);
		virtual ~MotorSerial();
		bool sumCheckSuccess;
		short recentReceiveData;
		bool serialReceiveSuccess;
	private:
		void sendingLoop(void);
		static bool nowSendingFlag;
		static int timeOut;
		static int serialFile;
		static char *serialFileName;
		static int bRate;
		static int redePin;
		static bool threadLoopFlag;
		static std::thread sendThread;
		static std::queue<sendDataFormat> sendDataQueue;
	};

}
