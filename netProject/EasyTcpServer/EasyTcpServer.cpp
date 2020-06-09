#include "EasyTcpServer.h"

EasyTcpServer::EasyTcpServer() {
	_sock = INVALID_SOCKET;
	_tTime = CELLTimestamp();
	_recvCount = 0;
	_msgCount = 0;
};

int EasyTcpServer::InitSocket() {
	if (INVALID_SOCKET != _sock) {
		Close();
	}

#ifdef  _WIN32
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif //  _WIN32

	if (INVALID_SOCKET == _sock) {
		Close();
	}

	//��socket Api ��������TCP�����  //�������׽��� BSD socket
	//1������һ��socket �׽���(���ｨ������ ��������ͨ��(ipv4)�е��ֽ���ͨ�ŵ�TCPЭ�飩
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == _sock) {
		printf("����Socket<%d>����ʧ��\n", _sock);
		return false;
	}
	else {
		return true;
	}
};

//�󶨶˿�
int EasyTcpServer::Bind(unsigned short port) {
	if (INVALID_SOCKET == _sock) {
		InitSocket();
	}
	//2��bind �����ڽ��տͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);        //host to net unsigned short;  �����ֽ���
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");   //(INADDR_ANY)
#else
	_sin.sin_addr.s_addr = inet_addr("127.0.0.1");   //(INADDR_ANY)
#endif // _WIN32

	printf("sizeof(sockaddr_in):%d sizeof(sockaddr): %d\n", sizeof(sockaddr_in), sizeof(sockaddr));

	if (SOCKET_ERROR == bind(_sock, (sockaddr *)&_sin, sizeof(_sin))) {
		printf("�󶨳���<%d>\n", _sock);
		return -1;
	}
	else {
		printf("�󶨳ɹ�<%d>\n", _sock);
		return 1;
	}
};

//�����˿�	listen ��������˿� (�ڶ���������tcp���Ӷ��еĴ�С��һ����1-5)��
int EasyTcpServer::Listen(int n) {
	if (SOCKET_ERROR == listen(_sock, n)) {
		printf("����ʧ��<%d>\n", _sock);
		return -1;
	}
	else {
		printf("�����ɹ�<%d>\n", _sock);
		return 1;
	}
};

//����߳�
void EasyTcpServer::StartAddThread(int count) {
	for (int i = 0; i < count; i++) {
		CellServer * cell = new CellServer(_sock);
		_cellServers.push_back(cell);
		cell->setEventObj(this);
		cell->start();
	}
};

//����������Ϣ
bool EasyTcpServer::OnRun() {
	if (!IsRun()) return false;
	time4msg();

	fd_set fdRead; //��������socket������
	FD_ZERO(&fdRead);
	//����������socket�����뵽������
	FD_SET(_sock, &fdRead);

	//����ʽ
	//int rec = select(_sock + 1, &fdRead, &fdWrite, &fdExpect, NULL);
	//select�����е�һ������nfds�� ��һ������ֵ  ��ָfd_set������������������socket���ķ�Χ������������
	//���������ļ����������ֵ + 1����windows�������������д0��
	//������ʽ
	timeval time = { 0,10};   //�ӳ�Ϊ 0�룬0 ���� (����һ�����ĵȴ�ʱ������һ����ͣ��ô��)
	int rec = select(_sock + 1, &fdRead, NULL, NULL, &time);   //rec:0 û���յ�����   1���յ�����
																			//printf("rec = %d\n", rec);
	if (rec < 0) {
		printf("�յ���Ϣ����");
		Close();
		return false;
	}

	//�ж���������socket���Ƿ��ڼ�����
	//����Ҽ���
	if (FD_ISSET(_sock, &fdRead)) {
		FD_CLR(_sock, &fdRead);
		AcceptClient();
		return true;
	}
};

//���տͻ�������
int EasyTcpServer::AcceptClient() {
	sockaddr_in clientAddr = {};
	int addrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;   //(��Ч��socket)��
#ifdef _WIN32
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &addrLen);
#else
	_cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&addrLen);
#endif // _WIN32


	if (INVALID_SOCKET == _cSock)
	{
		printf("���յ���Ч�ͻ���");
		return -1;
	}
	else
	{
		ClientSocket *client = new ClientSocket(_cSock);
		addClientToCellThread(client);
		return 1;
	}
};

void EasyTcpServer::addClientToCellThread(ClientSocket * _cSocket) {
	CellServer* pMinServer = _cellServers[0];
	for (int i = 0; i < _cellServers.size(); i++) {
		if (pMinServer->getClientCount() > _cellServers[i]->getClientCount()) {
			pMinServer = _cellServers[i];
		}
	}
	pMinServer->addClient(_cSocket);
};

//��������
int EasyTcpServer::Send(SOCKET _cSock, DataHead *data) {
	if (IsRun() && data) {
		return send(_cSock, (char*)data, data->dataLen, 0);
	}

	return SOCKET_ERROR;
};

//���㲢���ÿ���յ���������Ϣ
void EasyTcpServer::time4msg()
{
	auto t1 = _tTime.getElapsedSecond();
	if (t1 >= 1.0)
	{
		printf("thread<%d>,time<%lf>,socket<%d>,clients<%d>,recvCount<%d>,msg<%d>\n",_cellServers.size(), t1, _sock, _clientCount.load(), (int)(_recvCount.load() / t1),(int)(_msgCount.load( ) / t1));
		_recvCount=0;
		_msgCount = 0;
		_tTime.update();
	}
};

//�ر�Socket
void EasyTcpServer::Close() {
	if (_sock != INVALID_SOCKET)
	{
#ifdef _WIN32
		//�ر��׽���closesocket
		closesocket(_sock);
		//------------
		//���Windows socket����
		WSACleanup();
#else
		//�ر��׽���closesocket
		close(_sock);
#endif
	}
};

//�Ƿ��ڹ�����
bool EasyTcpServer::IsRun() {
	if (INVALID_SOCKET == _sock) return false;
	return true;
};


void EasyTcpServer::OnNetJoin(ClientSocket* pClient) {
	_clientCount++;
};

void EasyTcpServer::OnNetLeave(ClientSocket* pClient) {
	_clientCount--;
};

void EasyTcpServer::OnNetRecv(ClientSocket* pClient) {
	_recvCount++;
};

void EasyTcpServer::OnNetMsg(CellServer *pCellServer,ClientSocket* pClient, DataHead* header) {
	_msgCount++;
};

