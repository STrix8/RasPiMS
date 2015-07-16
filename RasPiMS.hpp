#pragma once

#include <fstream>
#include <thread>

namespace RPMS {
	extern const char *SerialPortOpenError;
	extern const char *WiringPiSetupError;
	extern const char *SerialError;
	typedef struct {
		unsigned char id;
		unsigned char cmd;
		short argData;
	}sendDataFormat;
	class MotorSerial {
	public:
		MotorSerial(int, double timeout = 0.01, const char *serialFileName = "/dev/ttyAMA0", int bRate = 115200);
		MotorSerial();
		short send(unsigned char, unsigned char, short, bool multiThread = false);
		short send(sendDataFormat, bool multiThread = false);
		virtual ~MotorSerial();
		short sending(unsigned char, unsigned char, short);
	private:
		static bool nowSendingFlag;
		static double timeOut;
		static int serialFile;
		int redePin;
		static std::thread sendThread;
	};
}
