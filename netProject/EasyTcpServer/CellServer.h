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
���fdRead �Ǵ��ڿͻ��˵� _cSocket ,��˵������ͻ��������ݷ���(�����˶Ͽ����� �� ���͹�������������)

Ȼ����н���
int len = recv(_cSocket, szRecv, 0, 0);    ����յ���len���� == -1 ��˵���ÿͻ����Ѿ��˳� ����Ϊ�յ����ֽ���
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
	//������
	char _szRecv[RECV_BUFF_SZIE] = {};
	//����ͻ�����
	std::vector<ClientSocket*> _clientsBuff;
	//ʵ�ʿͻ����� ��map �ĺô��ǲ��ұȽϿ�
	std::map<SOCKET,ClientSocket*> _clients;
	fd_set _fdRead_bak; //��������socket������
	bool _clients_change = false;  //�Ƿ����¿ͻ��˼���
	SOCKET _maxSock = INVALID_SOCKET;
	SOCKET _sock = INVALID_SOCKET;
	std::thread _thread;
	std::thread _mainThread;
	INetEvent *_pNetEvent;

	std::mutex _mutex;
	CellTaskServer _taskServer;
};

