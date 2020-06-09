#ifndef _ClientSocket_hpp_
#define _ClientSocket_hpp_


#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#ifdef _WIN32
#define FD_SETSIZE 2506
#include<windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string>.h
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif // _WIN32

#ifndef RECV_BUFF_SZIE
#define RECV_BUFF_SZIE 10240
#define SEND_BUFF_SZIE RECV_BUFF_SZIE * 5
#endif // !1

#include "EasyTcpMassage.h"



class ClientSocket
{
public:
	ClientSocket(SOCKET sockfd = INVALID_SOCKET) {
		_sockfd = sockfd;
		memset(_buf, 0, sizeof(_buf));
		_lastPos = 0;
		_sendLastPos = 0;
	};
	char* getBuf() {
		return _buf;
	};
	int getLastPos() {
		return _lastPos;
	};
	SOCKET getSockfd() {
		return _sockfd;
	};
	void setLastPos(int lastPos) {
		_lastPos = lastPos;
	};
	//·¢ËÍÊý¾Ý
	int SendData(DataHead* header)
	{
		SOCKET ret = SOCKET_ERROR;
		if (header)
		{
			int dataLen = header->dataLen;
			if (_sendLastPos + dataLen >= SEND_BUFF_SZIE) {
				int copyLen = SEND_BUFF_SZIE - _sendLastPos;
				memcpy(_sendBuf + _sendLastPos, header, copyLen);
				ret = send(_sockfd,_sendBuf, SEND_BUFF_SZIE, 0);
				if (ret == SOCKET_ERROR) return ret;
				_sendLastPos = dataLen - copyLen;
				memcpy(_sendBuf, header, _sendLastPos);
			}
			else {
				memcpy(_sendBuf + _sendLastPos, header, dataLen);
				_sendLastPos += dataLen;
				//break;
			}
		}
		return SOCKET_ERROR;
	}
	virtual ~ClientSocket() {};

private:
	SOCKET _sockfd;
	char _buf[RECV_BUFF_SZIE * 5];
	int _lastPos;

	char _sendBuf[RECV_BUFF_SZIE * 5];
	int _sendLastPos;
};

#endif // !_CLIENTSOCKET_HPP_