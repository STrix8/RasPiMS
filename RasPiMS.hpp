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
		MotorSerial(int, double timeout = 0.01, const char *devFileName = "/dev/ttyAMA0", int bRate = 115200);
		MotorSerial();
		void init();
		short send(unsigned char, unsigned char, short, bool multiThread = false);
		short send(sendDataFormat, bool multiThread = false);
		short sending(unsigned char, unsigned char, short);
		virtual ~MotorSerial();
	private:
		static bool nowSendingFlag;
		static double timeOut;
		static int serialFile;
		static char *serialFileName;
		static int bRate;
		int redePin;
		static std::thread sendThread;
		static bool sumCheckSuccess;
	};
}
