#pragma once

#include <fstream>
#include <thread>

const char *SerialPortOpenError = "Cannot open serialport.";
const char *WiringPiSetupError = "WiringPi Setup Error.";

namespace RPMS {
	typedef struct {
		unsigned char id;
		unsigned char cmd;
		short argData;
	}sendDataFormat;
	class MotorSerial {
	public:
		MotorSerial(int, double timeOut = 0.01, const char *serialFileName = "/dev/ttyAMA0", int bRate = 115200);
		MotorSerial();
		short send(unsigned char, unsigned char, short, bool multiThread = false);
		short send(sendDataFormat, bool multiThread = false);
		virtual ~MotorSerial();
	private:
		short sending(unsigned char, unsigned char, short);
		static bool nowSendingFlag;
		static double timeOut;
		static int serial;
		int redePin;
		static std::thread sendThread;
	};
}
