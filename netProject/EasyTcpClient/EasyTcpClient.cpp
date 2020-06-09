#include "EasyTcpClient.h";

//��ʼ��socket
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
		printf("��ʼ��socket ʧ��\n");
		return false;
	}
	else
	{
		return true;
	}
};

	//���ӷ�����
int EasyTcpClient::Connect(char * ip, short porst){
		if (INVALID_SOCKET == _sock) {
			InitSocket();
		}
		//2�����ӷ����� connect
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
			printf("����ʧ�� _sock = %d\n", _sock);
			return -1;
		}
		else {
			return 1;
		}
};

//��Ϣ�ܴ���
void EasyTcpClient::OnRun() {
	if (IsRun()) {
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);

		timeval time = { 0,0 };
		int res = select(_sock + 1, &fdRead, NULL, NULL, &time);
		if (res < 0) {
			printf("�������");
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

//������Ϣ
int EasyTcpClient::RecvData()
{
	int len = recv(_sock, _szRecv, sizeof(DataHead), 0);    //len  ����ĳ���
	if (len <= 0)
	{
		printf("�ͻ���socket = %d ���˳������������\n", _sock);
		return -1;
	}
	//�ѽ��յ������ݷŵ���������
	memcpy(_szMsgRecv + lastPos, _szRecv, len);

	lastPos += len;
	while (lastPos >= sizeof(DataHead))
	{
		//��������Ϣ
		DataHead *header = (DataHead *)_szMsgRecv;
		int dataLen = header->dataLen;
		//printf("�յ�����˷���������Ϊ��%s,����Ϊ��%d\n", getCmdName(header->cmd), header->dataLen);
		if (lastPos < dataLen) return 1;

		OnNetMsg(header);

		//�Ѻ�������ݸ��Ƶ�ǰ������
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
		//printf("�յ�����˷����ĵ�¼ȷ����ϢΪ��%d\n", login->result);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* loginOut = (LogoutResult*)header;
		//printf("�յ�����˷�������ϢΪ��%d\n", loginOut->result);
		//�����յ��û��˳���ķ�����Ϣ��
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* loginOut = (NewUserJoin*)header;
		//printf("��������Ҽ��룺curSocket:%d otherSocket:%d\n", _sock,loginOut->socket);
	}
	break;
	}
};

//������Ϣ
int EasyTcpClient::SendData(DataHead * data,int len) {
	int ret = SOCKET_ERROR;
	if (IsRun() && data)
	{
		return send(_sock, (char*)data, len, 0);

		//����send����
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

//�Ƿ�����״̬
bool EasyTcpClient::IsRun() {
	return _sock != INVALID_SOCKET;
};

//�ر�socket
void EasyTcpClient::Close()
{
	if (INVALID_SOCKET == _sock) return;
#ifdef  _WIN32
	//4���ر��׽��� closesocket
	closesocket(_sock);

	//���sock ����
	WSACleanup();
	_sock = INVALID_SOCKET;
#else
	Close(_sock);
	_sock = INVALID_SOCKET;
#endif
};