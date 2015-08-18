test: test.o RasPiMS.o
	g++ -Wall -o test test.o RasPiMS.o -lwiringPi -std=c++11 -pthread
RasPiMS.o: RasPiMS.cpp
	g++ -Wall -c RasPiMS.cpp -lwiringPi -std=c++11 -pthread
test.o: test.cpp
	g++ -Wall -c test.cpp -lwiringPi -std=c++11 -pthread
clean:
	rm -f *.o test
