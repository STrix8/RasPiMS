#pragma once

#include <fstream>
#include <thread>

namespace RPMS {
	typedef struct {
		unsigned char id;
		unsigned char cmd;
		short argData;
	}sendDataFormat;
	class MoterSerial {
	public:
		MoterSerial(int, double timeOut = 0.01, char *serialFileName = "/dev/ttyAMA0", int bRate = 115200);
		MoterSerial();
		short send(unsigned char, unsigned char, short, bool multiThread = false);
		short send(sendDataFormat, bool multiThread = false);
		virtual ~MoterSerial();
	private:
		short sending(unsigned char, unsigned char, short);
		static bool nowSendingFlag;
		static double timeOut;
		static int serial;
		int redePin;
		static std::thread sendThread;
	}
}
