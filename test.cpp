#include <iostream>
#include "RasPiMS.hpp"

using namespace std;
using namespace RPMS;

int main(int argc, char *argv[]) {
	MotorSerial ms;
	try {
		ms.init();
	}
	catch(const char *str) {
		cout << str << endl;
		return -1;
	}
	unsigned char id, cmd;
	unsigned short data;
	if (argc < 4) {
		id = 1;
		cmd = 2;
		data = 255;
	} else {
		id = (unsigned char)stoi(argv[1]);
		cmd = (unsigned char)stoi(argv[2]);
		data = (short)stoi(argv[3]);
	}
	cout << (int)id << " " << (int)cmd << " " << (int)data << endl;
	try {
		cout << ms.sending(id, cmd, data) << endl;
	}
	catch(const char *str) {
		cout << str << endl;
		return -1;
	}
	return 0;
}
