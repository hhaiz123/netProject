
#include "CellServer.h"

CellServer::CellServer(SOCKET sockfd) {
	_sock = sockfd;
};

void CellServer::start()
{
	_thread = std::thread(std::mem_fn(&CellServer::OnRun), this);
	_taskServer.start();
};

bool CellServer::OnRun() {
	while (IsRun()) {
		//��ӿͻ���
		if (_clientsBuff.size() >0) {
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto itr : _clientsBuff) {
				_clients[itr->getSockfd()] = itr;
			}
			_clientsBuff.clear();
			_clients_change = true;
		}

		if (_clients.empty()) {
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}

		fd_set fdRead;
		FD_ZERO(&fdRead);
		if (_clients_change) {
			_clients_change = false;
			_maxSock = _clients.begin()->first;
			for (auto itr : _clients) {
				FD_SET(itr.first, &fdRead);
				if (_maxSock < itr.first)
				{
					_maxSock = itr.first;
				}
			}
			memcpy(&_fdRead_bak, &fdRead, sizeof(fd_set));
		}
		else {
			memcpy(&fdRead, &_fdRead_bak, sizeof(fd_set));
		}


		//����ʽ
		//int rec = select(_sock + 1, &fdRead, &fdWrite, &fdExpect, NULL);
		//select�����е�һ������nfds�� ��һ������ֵ  ��ָfd_set������������������socket���ķ�Χ������������
		//���������ļ����������ֵ + 1����windows�������������д0��
		//������ʽ
		timeval time = { 0,0 };   //�ӳ�Ϊ 0�룬0 ���� (����һ�����ĵȴ�ʱ������һ����ͣ��ô��)
		int rec = select(_maxSock + 1, &fdRead, nullptr, nullptr, nullptr);   //rec:0 û���յ�����   1���յ�����
																	  //printf("rec = %d\n", rec);
		if (rec < 0) {
			printf("select���������\n");
			Close();
			return false;
		}
		else if (rec == 0)
		{
			continue;
		}

		for (int i = 0; i < fdRead.fd_count; i++) {
			auto itr = _clients.find(fdRead.fd_array[i]);
			if (itr != _clients.end()) {
				//ɾ��
				if (-1 == RecvData(itr->second)) {
					_clients_change = true;
					_clients.erase(itr->first);
				}
			}else {
					printf("error. if (iter != _clients.end())\n");
			}
		}
	}
};

void CellServer::addClient(ClientSocket* _clientSocket) {
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuff.push_back(_clientSocket);
	_pNetEvent->OnNetJoin(_clientSocket);
};

int CellServer::RecvData(ClientSocket *_client) {
	//����recv����
	//int len = (int)recv(_client->getSockfd(), _szRecv, 1, 0);
	//int len = (int)recv(_client->getSockfd(), _szRecv, RECV_BUFF_SZIE, 0);
	//�������Ż�
	int len = (int)recv(_client->getSockfd(), _client->getBuf() + _client->getLastPos(), RECV_BUFF_SZIE * 5 - _client->getLastPos(), 0);
	if (len <= 0)
	{
		//printf("�ͻ���socket = %d ���˳������������\n", _client->getSockfd());
		_pNetEvent->OnNetLeave(_client);
		return -1;
	}
	_pNetEvent->OnNetRecv(_client);

	//memcpy(_client->getBuf() + _client->getLastPos(), _szRecv, len);
	_client->setLastPos(_client->getLastPos() + len);

	//�ж���Ϣ�����������ݳ��ȴ�����ϢͷDataHeader����
	while (_client->getLastPos() >= sizeof(DataHead)) {
		DataHead *head = (DataHead *)_client->getBuf();
		int length = head->dataLen;

		//�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
		if (_client->getLastPos() < length) break;

		//printf("�յ��ͻ���<%d>����������Ϊ��%s,����Ϊ��%d\n", _client->getSockfd(), getCmdName(head->cmd), head->dataLen);
		_pNetEvent->OnNetMsg(this,_client, head);
		int nSize = _client->getLastPos() - length;
		memcpy(_client->getBuf(), _client->getBuf() + length, nSize);
		_client->setLastPos(nSize);
	};
	return 0;
};

void CellServer::addSendTask(ClientSocket* pClient, DataHead* header) {
	CellSendMsg2ClientTask *task = new CellSendMsg2ClientTask(pClient, header);
	_taskServer.addTask(task);
}

int CellServer::getClientCount() {
	return _clients.size() + _clientsBuff.size();
};

void CellServer::setEventObj(INetEvent* event)
{
	_pNetEvent = event;
};

void CellServer::Close() {
	if (INVALID_SOCKET == _sock) return;
#ifdef  _WIN32
	for (auto itr : _clients) {
		closesocket(itr.first);
	}
	//6���ر��׽��� closesocket 
	closesocket(_sock);
	WSACleanup();
#else
	for (auto itr : _clients) {
		close(itr.first);
	}
	close(_sock);
#endif
	_clients.clear();
};

bool CellServer::IsRun() {
	return _sock != INVALID_SOCKET;
};