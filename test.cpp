#include <iostream>
#include "RasPiMS.hpp"

using namespace std;
using namespace RPMS;

int main(void) {
	MotorSerial ms;
	int id, cmd, data;
	cin >> id >> cmd >> data;
	cout << ms.send(id, cmd, data) << endl;
	return 0;
}
