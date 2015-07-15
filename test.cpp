#include <iostream>
#include "RasPiMS.hpp"

using namespace std;
using namespace RPMS;

int main(void) {
	MoterSerial ms;
	cout << id cmd data << endl;
	sendDataFormat dat = {};
	cin >> (int)dat.id >> (int)dat.cmd >> dat.data;
	cout << ms.send(dat); << endl;
	return 0;
}
