#include<iostream>
#include<thread>
#include<atomic>
#include"EasyTcpClient.h"
#include"CELLTimestamp.hpp"

bool g_bRun = true;
const int clientCount = 10000;
const int threadCount = 4;
std::atomic_int readyCount = 0;
std::atomic_int sendCount = 0;
std::atomic_int curClientCount = 0;
EasyTcpClient* clientArr[clientCount];


//输入线程
void cmdThread() {
	while (true) {
		char recBuf[256];
		scanf("%s", recBuf);

		if (0 == strcmp(recBuf, "exit")) {
			printf("退出cmdThread\n");
			g_bRun = false;
			break;
		}
		else {
			printf("输入错误，请重新输入");
		}
	}
};

//连接 发送线程
void sendDataThread(int idx) {
	int distanceC = clientCount / threadCount;
	int begin = idx * distanceC;
	int end = (idx + 1) * distanceC;

	//连接
	for (int i = begin; i < end; i++) {
		clientArr[i] = new EasyTcpClient();
		clientArr[i]->Connect("127.0.0.1", 4567);
		curClientCount++;
	}

	printf("thread<%d>,Connect<begin=%d, end=%d>\n", idx, begin, end);
	readyCount++;
	while (readyCount < threadCount){
		//等待其它线程准备好发送数据
		std::chrono::milliseconds t(10);
		std::this_thread::sleep_for(t);
	}

	//发送消息
	Login login[1];
	for (int i = 0; i < 1; i++) {
		strcpy(login[i].userName, "hhaiz");
		strcpy(login[i].PassWord, "123456");
	}

	int len = sizeof(login);
	
	while (g_bRun) {
		for (int i = begin; i < end; i++) {
			if (SOCKET_ERROR != clientArr[i]->SendData(login, len)) {
				sendCount ++;
			}
		}
	}

	for (int i = begin; i < end; i++) {
		clientArr[i]->Close();
	}

	printf("thread<%d>,exit\n",idx);
};

int main() {
	CELLTimestamp tTime;
	std::thread cmdT(cmdThread);
	cmdT.detach();

	for (int i = 0; i < threadCount; i++) {
		std::thread t1(sendDataThread,i);
		t1.detach();
	}

	while (g_bRun) {
		auto t = tTime.getElapsedSecond();
		if (t >= 1.0) {
			printf("thread<%d>,clients<%d>,time<%lf>,send<%d>\n", threadCount, curClientCount.load(), t, (int)(sendCount.load() / t));
			sendCount = 0;
			tTime.update();
		}
		Sleep(1);
	}


	getchar();
	return 1;
};