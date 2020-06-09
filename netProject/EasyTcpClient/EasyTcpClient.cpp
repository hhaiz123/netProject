#include "EasyTcpClient.h";

//初始化socket
bool EasyTcpClient::InitSocket() {
#ifdef  _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif //  _WIN32

	if (_sock != INVALID_SOCKET) {
		Close();
	}
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		printf("初始化socket 失败\n");
		return false;
	}
	else
	{
		return true;
	}
};

	//连接服务器
int EasyTcpClient::Connect(char * ip, short porst){
		if (INVALID_SOCKET == _sock) {
			InitSocket();
		}
		//2、连接服务器 connect
		sockaddr_in serverAddr = {};
#ifdef  _WIN32
		serverAddr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		serverAddr.sin_addr.s_addr = inet_addr(ip);
#endif
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(porst);
		int connectFlag = connect(_sock, (sockaddr*)&serverAddr, sizeof(sockaddr));
		if (SOCKET_ERROR == connectFlag) {
			_sock = INVALID_SOCKET;
			printf("连接失败 _sock = %d\n", _sock);
			return -1;
		}
		else {
			return 1;
		}
};

//消息总处理
void EasyTcpClient::OnRun() {
	if (IsRun()) {
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);

		timeval time = { 0,0 };
		int res = select(_sock + 1, &fdRead, NULL, NULL, &time);
		if (res < 0) {
			printf("任务结束");
			Close();
		}

		if (FD_ISSET(_sock, &fdRead)) {
			FD_CLR(_sock, &fdRead);
			if (-1 == RecvData()) {
				Close();
			}
		}
	}
};

//接收信息
int EasyTcpClient::RecvData()
{
	int len = recv(_sock, _szRecv, sizeof(DataHead), 0);    //len  缓存的长度
	if (len <= 0)
	{
		printf("客户端socket = %d 已退出，任务结束。\n", _sock);
		return -1;
	}
	//把接收到的数据放到处理缓存区
	memcpy(_szMsgRecv + lastPos, _szRecv, len);

	lastPos += len;
	while (lastPos >= sizeof(DataHead))
	{
		//处理单个消息
		DataHead *header = (DataHead *)_szMsgRecv;
		int dataLen = header->dataLen;
		//printf("收到服务端发来的命令为：%s,长度为：%d\n", getCmdName(header->cmd), header->dataLen);
		if (lastPos < dataLen) return 1;

		OnNetMsg(header);

		//把后面的数据复制到前面来。
		memcpy(_szMsgRecv, _szMsgRecv + dataLen, lastPos - dataLen);
		lastPos = lastPos - dataLen;
	}

	return 1;
};

void EasyTcpClient::OnNetMsg(DataHead * header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* login = (LoginResult*)header;
		//printf("收到服务端发来的登录确认信息为：%d\n", login->result);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* loginOut = (LogoutResult*)header;
		//printf("收到服务端发来的信息为：%d\n", loginOut->result);
		//发送收到用户退出后的反馈信息。
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* loginOut = (NewUserJoin*)header;
		//printf("有其他玩家加入：curSocket:%d otherSocket:%d\n", _sock,loginOut->socket);
	}
	break;
	}
};

//发送信息
int EasyTcpClient::SendData(DataHead * data,int len) {
	int ret = SOCKET_ERROR;
	if (IsRun() && data)
	{
		return send(_sock, (char*)data, len, 0);

		//测试send性能
		/*for (int i = 0; i < len; i++) {
			ret = send(_sock, (char*)data + i, 1, 0);
		}

		if (SOCKET_ERROR == ret)
		{
			Close();
		}
		return ret;*/
	}
	return SOCKET_ERROR;
};

//是否连接状态
bool EasyTcpClient::IsRun() {
	return _sock != INVALID_SOCKET;
};

//关闭socket
void EasyTcpClient::Close()
{
	if (INVALID_SOCKET == _sock) return;
#ifdef  _WIN32
	//4、关闭套节字 closesocket
	closesocket(_sock);

	//清除sock 环境
	WSACleanup();
	_sock = INVALID_SOCKET;
#else
	Close(_sock);
	_sock = INVALID_SOCKET;
#endif
};