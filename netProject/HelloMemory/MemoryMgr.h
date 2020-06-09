#pragma once
#ifndef MAX_MEMORY_SZIE
#define MAX_MEMORY_SZIE 1024
#endif // !MAX_MEMORY_SZIE


class MemoryAlloc;
class MemoryMgr;

static MemoryMgr * _mgr = nullptr;

//�ڴ�� ��С��Ԫ
class MemoryBlock {
public:
	MemoryBlock();

	~MemoryBlock();
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
	MemoryAlloc();
	~MemoryAlloc();
	//�����ڴ�
	void* allocMemory(size_t size);

	//�ͷ��ڴ�
	void freeMemory(void* pMem);

	//��ʼ��
	void initMemory();
protected:
	//�ڴ�ص�ַ
	char* _pBuf;
	//ͷ���ڴ浥Ԫ
	MemoryBlock * _pHead;
	//�ڴ浥Ԫ�Ĵ�С
	size_t _nSize;
	//�ڴ浥Ԫ������
	size_t _nBlockCount;
};

template<size_t nSize,size_t nBlockCount>
class MemoryAlloctor: public MemoryAlloc {
public:
	MemoryAlloctor();
};

//�ڴ������
class MemoryMgr {
public:
	static MemoryMgr* getInstance();
	void init();
	//�����ڴ�
	void* allocMem(size_t nSize);
	//�ͷ��ڴ�
	void freeMem(void *pMem);
private:
	//��ʼ���ڴ��ӳ������
	void init_szAlloc(int nBegin, int nEnd, MemoryAlloc* pMemA);

private:
	MemoryAlloctor<64, 10> _pMem1;
	MemoryAlloctor<128, 10> _pMem2;
	MemoryAlloctor<256, 10> _pMem3;
	MemoryAlloctor<512, 10> _pMem4;
	MemoryAlloctor<1024, 10> _pMem5;

	MemoryAlloc* _szAlloc[MAX_MEMORY_SZIE + 1];
};