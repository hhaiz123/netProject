#include <iostream>
#include <assert.h>

class MemoryAlloc;

//内存块 最小单元
class MemoryBlock {
public:
	MemoryBlock() {
		
	}

	~MemoryBlock() {

	}
public:
	//内存块编号
	int nID;
	//引用次数
	int nRef;
	//是否在内存池中
	bool bPool;
	//下一块位置
	MemoryBlock * pNext;
	//所属大内存块（池）
	MemoryAlloc *pAlloc;
private:
	//预留
	char c1;
	char c2;
	char c3;
};

//内存池
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
	//申请内存
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

	//释放内存
	void freeMemory(void* pMem) {
		MemoryBlock* pBlock = (MemoryBlock*)pMem;

	}

	//初始化
	void initMemory() {
		assert(nullptr == _pBuf);
		size_t singleSize = _nSize + sizeof(MemoryBlock);
		size_t totleSize = singleSize * _nBlockCount;
		_pBuf = (char *)malloc(totleSize);

		//初始化内存池
		_pHead = (MemoryBlock *)_pBuf;
		_pHead->bPool = true;
		_pHead->nID = 0;
		_pHead->nRef = 0;
		_pHead->pAlloc = this;
		_pHead->pNext = nullptr;

		MemoryBlock * pLast = _pHead;
		for (int i = 1; i < _nBlockCount; i++) {
			MemoryBlock *pCur = (MemoryBlock*)(_pBuf + singleSize * i);
			printf("地址:%p\n", pCur);
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
	//内存池地址
	char* _pBuf;
	//头部内存单元
	MemoryBlock * _pHead;
	//内存单元的大小
	size_t _nSize;
	//内存单元的数量
	size_t _nBlockCount;
};

//内存管理工具
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

	//申请内存
	void* allMemory(size_t nSize) {
		return _test.allocMemory(nSize);
	}

	//释放内存
private:
	MemoryAlloc _test;
};



