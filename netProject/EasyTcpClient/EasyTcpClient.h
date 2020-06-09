#pragma once
#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#ifdef _WIN32
#include<windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string>.h
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif // _WIN32

#include<stdio.h>
#include"EasyTcpMassage.h"

#define RECV_BUF_SIZE 1024

class EasyTcpClient {
public:
	EasyTcpClient() {};
	virtual ~EasyTcpClient() {};

	bool InitSocket();
	int Connect(char * ip, short porst);
	void OnRun();
	int RecvData();
	void OnNetMsg(DataHead * header);
	int SendData(DataHead * data,int len);
	bool IsRun();
	void Close();
private:
	SOCKET _sock = INVALID_SOCKET;
	//接收缓冲区
	char _szRecv[RECV_BUF_SIZE] = {};
	//第二缓冲区  消息缓冲区
	char _szMsgRecv[RECV_BUF_SIZE * 10] = {};
	//消息缓冲区的数据尾部位置
	int lastPos = 0;
};

#endif;
