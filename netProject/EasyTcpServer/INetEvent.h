#pragma once

#include"EasyTcpMassage.h"
#include"ClientSocket.hpp"

class CellServer;

//网络事件接口
class INetEvent
{
public:
	//纯虚函数
	//客户端加入事件
	virtual void OnNetJoin(ClientSocket* pClient) = 0;
	//客户端离开事件
	virtual void OnNetLeave(ClientSocket* pClient) = 0;
	//客户端消息事件
	virtual void OnNetMsg(CellServer* pCellServer, ClientSocket* pClient, DataHead* header) = 0;
	//recv事件
	virtual void OnNetRecv(ClientSocket* pClient) = 0;
private:


};