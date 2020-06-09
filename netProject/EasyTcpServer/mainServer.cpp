#include "EasyTcpServer.h";


class MyServer:public EasyTcpServer {
public:
	MyServer() {};
	virtual ~MyServer() {};

	void OnNetJoin(ClientSocket* pClient) {
		_clientCount++;
	};

	void OnNetLeave(ClientSocket* pClient) {
		_clientCount--;
	};

	void OnNetRecv(ClientSocket* pClient) {
		_recvCount++;
	};

	void OnNetMsg(CellServer *pCellServer,ClientSocket* pClient, DataHead* header) {
		_msgCount++;
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{

			Login* login = (Login*)header;
			//printf("�յ��ͻ���<Socket=%d>����CMD_LOGIN,���ݳ��ȣ�%d,userName=%s PassWord=%s\n", cSock, login->dataLength, login->userName, login->PassWord);
			//�����ж��û������Ƿ���ȷ�Ĺ���
			LoginResult *ret = new LoginResult();
			pCellServer->addSendTask(pClient, ret);
		}
		break;
		case CMD_LOGOUT:
		{
			Logout* logout = (Logout*)header;
			//printf("�յ��ͻ���<Socket=%d>����CMD_LOGOUT,���ݳ��ȣ�%d,userName=%s \n", cSock, logout->dataLength, logout->userName);
			//�����ж��û������Ƿ���ȷ�Ĺ���
			//LogoutResult ret;
			//SendData(cSock, &ret);
		}
		break;
		default:
		{
			printf("<socket=%d>�յ�δ������Ϣ,���ݳ��ȣ�%d\n", pClient->getSockfd(), header->dataLen);
			//DataHeader ret;
			//SendData(cSock, &ret);
		}
		break;
		}
	};
};



int main()
{
	MyServer server;
	server.InitSocket();
	server.Bind(4567);
	server.Listen(5);
	server.StartAddThread(4);

	//2-17   1.3�汾����  select����������������������������������������������������������������������������������������������������������������������������������������������

	while (server.IsRun()) {
		server.OnRun();
	}


	printf("������˳����������");
	getchar();
}