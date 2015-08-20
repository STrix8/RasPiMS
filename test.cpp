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
	try {
		cout << ms.sending(3, 2, 255) << endl;
	}
	catch(const char *str) {
		cout << str << endl;
		return -1;
	}
	return 0;
}
