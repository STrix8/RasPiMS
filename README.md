# RasPiMoterSerial

先輩の作ったモータードライバとかのネットワークで便利に通信したいとおもって作った

WiringPiを使ってます C++11以降のコンパイラでビルドしてください

RPMS::MotorSerialクラスのインスタンスを作って使ってください

RPMS::MotorSerial(int ReDe = 4, double timeout = 0.01, consta char * devFileName = "/dev/ttyAMA0") REピンとDEピンの指定, タイムアウトの指定, デバイスのファイル名

void RPMS::MotorSerial::init() GPIOの初期化とか、シリアル通信のOpenなどを行います。インスタンスにつき一度だけ最初に呼んでください。

short RPMS::MotorSerial::sending(unsigned char id, unsigned char cmd, short data) id番の基板に対してcmd dataを送信します。返り値としてid番の基板から返ってきたdataを返します。

short RPMS::MotorSerial::send(unsigned char id, unsigned char cmd, short data, bool multiThread = false) multiThread = falseならsending()と同じ動作をします。multiThread = trueなら非同期で通信してくれます。その場合返り値は0です。
