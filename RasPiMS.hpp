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
		MoterSerial();
		MoterSerial(int, double timeOut = 0.01, char *devFileName = "/dev/ttyAMA0");
		short send(char, char, short);
		short send(sendDataFormat);
		virtual ~MoterSerial();
	private:
		static bool nowSendingFlag;
		static double timeOut;
		static std::fstream devFile;
		int redePin;
	}
}
