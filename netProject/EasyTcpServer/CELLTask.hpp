#pragma once
#include<list>
#include<thread>
#include<mutex>
#include"ClientSocket.hpp"
#include"EasyTcpMassage.h"

class CELLTask {
public:
	CELLTask() {

	}

	virtual void doTask() {

	}

	virtual ~CELLTask() {

	}
};


//执行任务的服务类型(用于处理发送)
class CellTaskServer {
public:
	CellTaskServer() {

	}

	void start() {
		std::thread t(std::mem_fn(&CellTaskServer::OnRun),this);
		t.detach();
	}

	void addTask(CELLTask * task) {
		std::lock_guard<std::mutex> lock(_mutex);
		_tasksBuf.push_back(task);
	}

	virtual ~CellTaskServer() {

	}
protected:
	void OnRun() {
		while (true) {
			if (!_tasksBuf.empty()) {
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pTask : _tasksBuf) {
					_tasks.push_back(pTask);
				}
				_tasksBuf.clear();
			}

			if (_tasks.empty()) {
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}


			for (auto pTask : _tasks) {
				pTask->doTask();
				delete pTask;
			}
			_tasks.clear();
		}
	}

private:
	std::list<CELLTask *> _tasks;
	std::list<CELLTask *> _tasksBuf;
	std::mutex _mutex;
};

class CellSendMsg2ClientTask : public CELLTask{
public:
	CellSendMsg2ClientTask(ClientSocket* pClient, DataHead* header) {
		_pClient = pClient;
		_header = header;
	}
	virtual ~CellSendMsg2ClientTask() {

	}

	void dotaTask() {
		_pClient->SendData(_header);
	}

private:
	ClientSocket * _pClient;
	DataHead * _header;
};


