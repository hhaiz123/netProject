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
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif //  _WIN32

	if (INVALID_SOCKET == _sock) {
		Close();
	}

	//用socket Api 建立简易TCP服务端  //伯克利套接字 BSD socket
	//1、建立一个socket 套接字(这里建立的是 基础网络通信(ipv4)中的字节流通信的TCP协议）
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == _sock) {
		printf("建立Socket<%d>连接失败\n", _sock);
		return false;
	}
	else {
		return true;
	}
};

//绑定端口
int EasyTcpServer::Bind(unsigned short port) {
	if (INVALID_SOCKET == _sock) {
		InitSocket();
	}
	//2、bind 绑定用于接收客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);        //host to net unsigned short;  网络字节序
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");   //(INADDR_ANY)
#else
	_sin.sin_addr.s_addr = inet_addr("127.0.0.1");   //(INADDR_ANY)
#endif // _WIN32

	printf("sizeof(sockaddr_in):%d sizeof(sockaddr): %d\n", sizeof(sockaddr_in), sizeof(sockaddr));

	if (SOCKET_ERROR == bind(_sock, (sockaddr *)&_sin, sizeof(_sin))) {
		printf("绑定出错<%d>\n", _sock);
		return -1;
	}
	else {
		printf("绑定成功<%d>\n", _sock);
		return 1;
	}
};

//监听端口	listen 监听网络端口 (第二个参数：tcp连接队列的大小，一般是1-5)；
int EasyTcpServer::Listen(int n) {
	if (SOCKET_ERROR == listen(_sock, n)) {
		printf("监听失败<%d>\n", _sock);
		return -1;
	}
	else {
		printf("监听成功<%d>\n", _sock);
		return 1;
	}
};

//添加线程
void EasyTcpServer::StartAddThread(int count) {
	for (int i = 0; i < count; i++) {
		CellServer * cell = new CellServer(_sock);
		_cellServers.push_back(cell);
		cell->setEventObj(this);
		cell->start();
	}
};

//处理网络消息
bool EasyTcpServer::OnRun() {
	if (!IsRun()) return false;
	time4msg();

	fd_set fdRead; //描述符（socket）集合
	FD_ZERO(&fdRead);
	//将描述符（socket）加入到集合中
	FD_SET(_sock, &fdRead);

	//阻塞式
	//int rec = select(_sock + 1, &fdRead, &fdWrite, &fdExpect, NULL);
	//select（）中第一个参数nfds： 是一个整数值  是指fd_set集合中所有描述符（socket）的范围，而不是数量
	//既是所有文件描述符最大值 + 1，在windows中这个参数可以写0；
	//非阻塞式
	timeval time = { 0,10};   //延迟为 0秒，0 毫秒 (这是一个最大的等待时，并不一定会停那么久)
	int rec = select(_sock + 1, &fdRead, NULL, NULL, &time);   //rec:0 没有收到数据   1：收到数据
																			//printf("rec = %d\n", rec);
	if (rec < 0) {
		printf("收到信息出错");
		Close();
		return false;
	}

	//判断描述符（socket）是否在集合中
	//新玩家加入
	if (FD_ISSET(_sock, &fdRead)) {
		FD_CLR(_sock, &fdRead);
		AcceptClient();
		return true;
	}
};

//接收客户端连接
int EasyTcpServer::AcceptClient() {
	sockaddr_in clientAddr = {};
	int addrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;   //(无效的socket)；
#ifdef _WIN32
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &addrLen);
#else
	_cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&addrLen);
#endif // _WIN32


	if (INVALID_SOCKET == _cSock)
	{
		printf("接收到无效客户端");
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

//发送数据
int EasyTcpServer::Send(SOCKET _cSock, DataHead *data) {
	if (IsRun() && data) {
		return send(_cSock, (char*)data, data->dataLen, 0);
	}

	return SOCKET_ERROR;
};

//计算并输出每秒收到的网络消息
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

//关闭Socket
void EasyTcpServer::Close() {
	if (_sock != INVALID_SOCKET)
	{
#ifdef _WIN32
		//关闭套节字closesocket
		closesocket(_sock);
		//------------
		//清除Windows socket环境
		WSACleanup();
#else
		//关闭套节字closesocket
		close(_sock);
#endif
	}
};

//是否在工作中
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

