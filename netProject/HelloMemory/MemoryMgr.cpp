#include <iostream>
#include <assert.h>
#include"MemoryMgr.h"



MemoryBlock::MemoryBlock() {
	nID = -1;
	nRef = 0;
	bPool = false;
	pNext = nullptr;
	pAlloc = nullptr;
}

MemoryBlock::~MemoryBlock() {

}

MemoryAlloc::MemoryAlloc() {
	_pBuf = nullptr;
	_pHead = nullptr;
	_nSize = 0;
	_nBlockCount = 0;
}
MemoryAlloc::~MemoryAlloc() {

}

void* MemoryAlloc::allocMemory(size_t size) {
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
	printf("allocMem: %llx, id=%d, size=%d\n", pReturn, pReturn->nID, size);
	return (char*)(pReturn) + sizeof(MemoryBlock);
}

void MemoryAlloc::freeMemory(void* pMem) {
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		assert(1 == pBlock->nRef);
		if (--pBlock->nRef != 0) {
			return;
		}
		if (pBlock->bPool) {
			pBlock->pNext = _pHead;
			_pHead = pBlock;
		}
		else {
			free(pBlock);
		}
	}

void MemoryAlloc::initMemory() {
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

template<size_t nSize, size_t nBlockCount>
MemoryAlloctor<nSize, nBlockCount>::MemoryAlloctor() {
	_nSize = nSize;
	_nBlockCount = nBlockCount;
};

MemoryMgr* MemoryMgr::getInstance () {
	if (nullptr == _mgr) {
		_mgr = new MemoryMgr();
		_mgr->init();
	}
	return _mgr;
}

void MemoryMgr::init() {
	init_szAlloc(0, 64, &_pMem1);
	init_szAlloc(65, 128, &_pMem2);
	init_szAlloc(129, 256, &_pMem3);
	init_szAlloc(256, 512, &_pMem4);
	init_szAlloc(512, 1024, &_pMem5);
};

void MemoryMgr::init_szAlloc(int nBegin, int nEnd, MemoryAlloc* pMemA) {
	pMemA->initMemory();
	for (int i = nBegin; i <= nEnd; i++) {
		_szAlloc[i] = pMemA;
	}
}

//申请内存
void* MemoryMgr::allocMem(size_t nSize) {
	if (nSize <= MAX_MEMORY_SZIE) {
		return _szAlloc[nSize]->allocMemory(nSize);
	}

	MemoryBlock* pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
	pReturn->bPool = false;
	pReturn->nID = -1;
	pReturn->nRef = 1;
	pReturn->pAlloc = nullptr;
	pReturn->pNext = nullptr;
	return (char*)pReturn + sizeof(MemoryBlock);
}
//释放内存
void MemoryMgr::freeMem(void *pMem) {
	MemoryBlock *pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
	if (pBlock->bPool) {
		pBlock->pAlloc->freeMemory(pMem);
		return;
	}
	if (--pBlock->nRef == 0) {
		free(pBlock);
	}

}






