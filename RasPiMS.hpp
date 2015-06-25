#pragma once

#include <fstream>

namespace RPMS {
	typedef struct {
		char id;
		char cmd;
		short argData;
	}sendDataFormat;
	class MoterSerial {
	public:
		MoterSerial(int, double timeOut = 0.01, char *serialFileName = "/dev/ttyAMA0", int bRate = 115200);
		MoterSerial();
		short send(char, char, short);
		short send(sendDataFormat);
		virtual ~MoterSerial();
	private:
		static bool nowSendingFlag;
		static double timeOut;
		static int serial;
		int redePin;
	}
}
