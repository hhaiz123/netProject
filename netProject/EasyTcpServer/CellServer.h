#pragma once

#include<vector>
#include<map>
#include<chrono>
#include<thread>
#include<mutex>
#include"ClientSocket.hpp"
#include"EasyTcpMassage.h"
#include"INetEvent.h"
#include"CELLTask.hpp"

#ifndef RECV_BUFF_SZIE
#define RECV_BUFF_SZIE 1024
#endif



/*
select(_maxSock + 1, &fdRead, nullptr, nullptr, nullptr) 
如果fdRead 是存在客户端的 _cSocket ,则说明这个客户端有数据返回(包含了断开连接 和 发送过来的其他数据)

然后进行接收
int len = recv(_cSocket, szRecv, 0, 0);    如果收到的len长度 == -1 则说明该客户端已经退出 否则为收到的字节数
*/

class CellServer {
public:
	CellServer(SOCKET sockfd = INVALID_SOCKET);
	void start();
	void addClient(ClientSocket* _clientSocket);
	bool OnRun();
	int RecvData(ClientSocket *_client);
	void addSendTask(ClientSocket* pClient, DataHead* header);
	int getClientCount();
	void setEventObj(INetEvent* event);
	void Close();
	bool IsRun();
	virtual ~CellServer() {};
private:
	//缓冲区
	char _szRecv[RECV_BUFF_SZIE] = {};
	//缓冲客户队列
	std::vector<ClientSocket*> _clientsBuff;
	//实际客户队列 用map 的好处是查找比较快
	std::map<SOCKET,ClientSocket*> _clients;
	fd_set _fdRead_bak; //描述符（socket）集合
	bool _clients_change = false;  //是否有新客户端加入
	SOCKET _maxSock = INVALID_SOCKET;
	SOCKET _sock = INVALID_SOCKET;
	std::thread _thread;
	std::thread _mainThread;
	INetEvent *_pNetEvent;

	std::mutex _mutex;
	CellTaskServer _taskServer;
};

