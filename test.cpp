#include <iostream>
#include "RasPiMS.hpp"

using namespace std;
using namespace RPMS;

int main(void) {
	MotorSerial ms;
	try {
		ms.init();
	}
	catch(const char *str) {
		cout << str << endl;
		return -1;
	}
	int id, cmd, data;
	//cin >> id >> cmd >> data;
	id = 2;
	cmd = 2;
	data = -255;
	try {
		cout << ms.sending(id, cmd, data) << endl;
	}
	catch(const char *str) {
		cout << str << endl;
		return -1;
	}
	return 0;
}
