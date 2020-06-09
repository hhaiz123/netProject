#pragma once

#include"EasyTcpMassage.h"
#include"ClientSocket.hpp"

class CellServer;

//�����¼��ӿ�
class INetEvent
{
public:
	//���麯��
	//�ͻ��˼����¼�
	virtual void OnNetJoin(ClientSocket* pClient) = 0;
	//�ͻ����뿪�¼�
	virtual void OnNetLeave(ClientSocket* pClient) = 0;
	//�ͻ�����Ϣ�¼�
	virtual void OnNetMsg(CellServer* pCellServer, ClientSocket* pClient, DataHead* header) = 0;
	//recv�¼�
	virtual void OnNetRecv(ClientSocket* pClient) = 0;
private:


};