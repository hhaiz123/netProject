#include <iostream>
#include <assert.h>

class MemoryAlloc;

//�ڴ�� ��С��Ԫ
class MemoryBlock {
public:
	MemoryBlock() {
		
	}

	~MemoryBlock() {

	}
public:
	//�ڴ����
	int nID;
	//���ô���
	int nRef;
	//�Ƿ����ڴ����
	bool bPool;
	//��һ��λ��
	MemoryBlock * pNext;
	//�������ڴ�飨�أ�
	MemoryAlloc *pAlloc;
private:
	//Ԥ��
	char c1;
	char c2;
	char c3;
};

//�ڴ��
class MemoryAlloc {
public:
	MemoryAlloc() {
		_pBuf = nullptr;
		_pHead = nullptr;
		_nSize = 64;
		_nBlockCount = 10;
	}
	~MemoryAlloc() {

	}
	//�����ڴ�
	void* allocMemory(size_t size) {
		if (!_pBuf)
		{
			initMemory();
		}

		MemoryBlock* pReturn = nullptr;
		if (nullptr == _pHead) {
			pReturn = (MemoryBlock*)malloc(size + sizeof(MemoryBlock));
			pReturn->bPool = false;
			pReturn->nID = 0;
			pReturn->nRef = 0;
			pReturn->pAlloc = this;
			pReturn->pNext = nullptr;
		}
		else {
			pReturn = _pHead;
			pReturn->nRef = 1;
			_pHead = _pHead->pNext;
		}
		printf("allocMem: %llx, id=%d, size=%ud\n", pReturn, pReturn->nID, size);
		return (char*)(pReturn) + sizeof(MemoryBlock);
	}

	//�ͷ��ڴ�
	void freeMemory(void* pMem) {
		MemoryBlock* pBlock = (MemoryBlock*)pMem;

	}

	//��ʼ��
	void initMemory() {
		assert(nullptr == _pBuf);
		size_t singleSize = _nSize + sizeof(MemoryBlock);
		size_t totleSize = singleSize * _nBlockCount;
		_pBuf = (char *)malloc(totleSize);

		//��ʼ���ڴ��
		_pHead = (MemoryBlock *)_pBuf;
		_pHead->bPool = true;
		_pHead->nID = 0;
		_pHead->nRef = 0;
		_pHead->pAlloc = this;
		_pHead->pNext = nullptr;

		MemoryBlock * pLast = _pHead;
		for (int i = 1; i < _nBlockCount; i++) {
			MemoryBlock *pCur = (MemoryBlock*)(_pBuf + singleSize * i);
			printf("��ַ:%p\n", pCur);
			pCur->bPool = true;
			pCur->nID = i;
			pCur->nRef = 0;
			pCur->pAlloc = this;
			pCur->pNext = nullptr;
			pLast->pNext = pCur;
			pLast = pCur;
		}
	}
private:
	//�ڴ�ص�ַ
	char* _pBuf;
	//ͷ���ڴ浥Ԫ
	MemoryBlock * _pHead;
	//�ڴ浥Ԫ�Ĵ�С
	size_t _nSize;
	//�ڴ浥Ԫ������
	size_t _nBlockCount;
};

//�ڴ������
class MemoryMgr {
public:
	static MemoryMgr& Instance() {
		static MemoryMgr mgr;
		return mgr;
	}

	MemoryMgr() {
		
		printf("%d",sizeof(_test));
	}
	~MemoryMgr() {

	}

	//�����ڴ�
	void* allMemory(size_t nSize) {
		return _test.allocMemory(nSize);
	}

	//�ͷ��ڴ�
private:
	MemoryAlloc _test;
};



