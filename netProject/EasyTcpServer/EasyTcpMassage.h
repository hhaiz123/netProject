#ifndef _MessageHeader_hpp_
#define _MessageHeader_hpp_

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHead
{
	DataHead()
	{
		dataLen = sizeof(DataHead);
		cmd = CMD_ERROR;
	}
	short dataLen;
	short cmd;
};

//DataPackage
struct Login : public DataHead
{
	Login()
	{
		dataLen = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
	char data[32];
};

struct LoginResult : public DataHead
{
	LoginResult()
	{
		dataLen = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
	char data[92];
};

struct Logout : public DataHead
{
	Logout()
	{
		dataLen = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogoutResult : public DataHead
{
	LogoutResult()
	{
		dataLen = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin : public DataHead
{
	NewUserJoin()
	{
		dataLen = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		scok = 0;
	}
	int scok;
};

#endif // !_MessageHeader_hpp_