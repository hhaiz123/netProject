#pragma once
#ifndef _EasyTcpServer_h_
#define _EasyTcpServer_h_

#include<stdio.h>
#include<vector>
#include<atomic>
#include"EasyTcpMassage.h"
#include "CELLTimestamp.hpp"
#include"ClientSocket.hpp"
#include"CellServer.h"
#include"INetEvent.h"

class EasyTcpServer: public INetEvent {
public:
	EasyTcpServer();
	int InitSocket();
	int Bind(unsigned short port);
	int Listen(int n);
	void StartAddThread(int count);
	int AcceptClient();
	bool OnRun();
	void addClientToCellThread(ClientSocket * _cSocket);
	void time4msg();
	void Close();
	bool IsRun();
	int Send(SOCKET _cSock, DataHead *data);

	virtual void OnNetJoin(ClientSocket* pClient);
	virtual void OnNetLeave(ClientSocket* pClient);
	virtual void OnNetMsg(CellServer *pCellServer,ClientSocket* pClient, DataHead* header);
	virtual void OnNetRecv(ClientSocket* pClient);

private:
	//std::vector<ClientSocket*> _clientArr;
	SOCKET _sock;
	std::vector<CellServer*>_cellServers;
	CELLTimestamp _tTime;
	int totalCount = 0;
protected:
	std::atomic<int> _recvCount = 0;
	std::atomic<int> _msgCount = 0;
	std::atomic<int> _clientCount = 0;
};


#endif