# RasPiMoterSerial

* 先輩の作ったモータードライバとかのネットワークで便利に通信したいとおもって作った
* 通信相手は[これ](https://github.com/okayu9/ScrpMotor)を書き込んだArduinoのUARTをRS485に変換したもののネットワークを想定しています
* WiringPiを使ってます
* threadとか使ってます C++11以降のコンパイラでビルドしてください
* RPMS::MotorSerialクラスのインスタンスを作って使ってください
* インスタンス一つにつきシリアルポート一つの気持ちで使ってね

## リファレンス的な何か
```cpp
RPMS::MotorSerial(int ReDe = 4, double timeout = 0.01, consta char * devFileName = "/dev/ttyAMA0")
```
* コンストラクタです.
* 引数は, REピンとDEピンの指定, タイムアウトの指定(ms), デバイスのファイル名

```cpp
void RPMS::MotorSerial::init()
```
* GPIOの初期化とか、シリアル通信のOpenなどを行います。インスタンスにつき一度だけ最初に呼んでください.

```cpp
short RPMS::MotorSerial::sending(unsigned char id, unsigned char cmd, short data)
```
* id番の基板に対してcmd, dataを送信します.返り値としてid番の基板から返ってきたdataを返します.

```cpp
short RPMS::MotorSerial::send(unsigned char id, unsigned char cmd, short data, bool asyncFlag = false)
```
* asyncFlag = falseならsending()と同じ動作をします.asyncFlag = trueなら非同期で通信してくれます。その場合返り値は0です.
* sendingよりもこっちを使っとけばタイプ数も少なくて済むし非同期・同期の切り替えも楽だよ.
* RaspberryPiはシングルコアなので, 非同期モードで短時間に大量のデータを送るようなコードを書くと通信を行うスレッドの処理が行われず,想定してるのと異なる動作をする場合があります.注意してください.

```cpp
bool RPMS::MotorSerial::sumCheckSuccess
```
* 直近の通信が受信のsumチェックの成功まで行ったかどうかの結果が入っています.

```cpp
short RPMS::MotorSerial::recentReceiveData
```
* 直近の通信で受信したdataが入っています.
* send()を非同期モードで使ったが結果がほしい時とかに使うといいのかもしれない.

```cpp
static bool RPMS::MotorSerial::wiringPiSetupGpioFlag
```
* wiringPiSetupGpio()を呼んだかどうかが入っています.
* init()より前にwiringPiSetupGpio()を呼んでいる場合とかにtrueを代入しておくと良いかもしれない.
