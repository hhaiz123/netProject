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
			//printf("收到客户端<Socket=%d>请求：CMD_LOGIN,数据长度：%d,userName=%s PassWord=%s\n", cSock, login->dataLength, login->userName, login->PassWord);
			//忽略判断用户密码是否正确的过程
			LoginResult *ret = new LoginResult();
			pCellServer->addSendTask(pClient, ret);
		}
		break;
		case CMD_LOGOUT:
		{
			Logout* logout = (Logout*)header;
			//printf("收到客户端<Socket=%d>请求：CMD_LOGOUT,数据长度：%d,userName=%s \n", cSock, logout->dataLength, logout->userName);
			//忽略判断用户密码是否正确的过程
			//LogoutResult ret;
			//SendData(cSock, &ret);
		}
		break;
		default:
		{
			printf("<socket=%d>收到未定义消息,数据长度：%d\n", pClient->getSockfd(), header->dataLen);
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

	//2-17   1.3版本报文  select、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、

	while (server.IsRun()) {
		server.OnRun();
	}


	printf("服务端退出，任务结束");
	getchar();
}