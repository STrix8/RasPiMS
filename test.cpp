#include <iostream>
#include "RasPiMS.hpp"

using namespace std;
using namespace RPMS;

int main(void) {
	MotorSerial *ms;	
	try{
		ms = new MotorSerial();
	}
	catch(const char *str) {
		cout << str << endl;
		return -1;
	}
	int id, cmd, data;
	cin >> id >> cmd >> data;
	cout << "send" << id << cmd << data << endl;
	try {
		cout << ms->sending(id, cmd, data) << endl;
	}
	catch(const char *str) {
		cout << str << endl;
		delete ms;
		return -1;
	}
	return 0;
}
