#include <iostream>
#include<thread>
#include<mutex>
#include<atomic>   //原子
#include"CELLTimestamp.hpp"


using namespace std;

//int sum = 0;
atomic<int> sum = 0;
const int threadCount = 4;
thread threadArr[threadCount];
mutex m;

void getTotal(int id) {
	for (int i = 0; i < 2000000; i++) {
		//自解锁
		//lock_guard<mutex> lg(m);

		//临界锁
		//m.lock();
		sum++;
		//m.unlock();
	}
};

int main() {

	for (int i = 0; i < threadCount; i++) {
		threadArr[i] = thread(getTotal, i);
	}

	CELLTimestamp tTime;
	for (int i = 0; i < threadCount; i++) {
		threadArr[i].join();
	}

	cout << tTime.getElapsedTimeInMilliSec() << ",sum= " << sum << endl;

	sum = 0; 
	tTime.update();
	for (int i = 0; i < 8000000; i++) {
		sum++;
	}

	cout << tTime.getElapsedTimeInMilliSec() << ",sum= " << sum << endl;

	getchar();
	return 0;
}