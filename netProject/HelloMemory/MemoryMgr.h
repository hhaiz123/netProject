#pragma once
#ifndef MAX_MEMORY_SZIE
#define MAX_MEMORY_SZIE 1024
#endif // !MAX_MEMORY_SZIE


class MemoryAlloc;
class MemoryMgr;

static MemoryMgr * _mgr = nullptr;

//内存块 最小单元
class MemoryBlock {
public:
	MemoryBlock();

	~MemoryBlock();
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
	MemoryAlloc();
	~MemoryAlloc();
	//申请内存
	void* allocMemory(size_t size);

	//释放内存
	void freeMemory(void* pMem);

	//初始化
	void initMemory();
protected:
	//内存池地址
	char* _pBuf;
	//头部内存单元
	MemoryBlock * _pHead;
	//内存单元的大小
	size_t _nSize;
	//内存单元的数量
	size_t _nBlockCount;
};

template<size_t nSize,size_t nBlockCount>
class MemoryAlloctor: public MemoryAlloc {
public:
	MemoryAlloctor();
};

//内存管理工具
class MemoryMgr {
public:
	static MemoryMgr* getInstance();
	void init();
	//申请内存
	void* allocMem(size_t nSize);
	//释放内存
	void freeMem(void *pMem);
private:
	//初始化内存池映射数组
	void init_szAlloc(int nBegin, int nEnd, MemoryAlloc* pMemA);

private:
	MemoryAlloctor<64, 10> _pMem1;
	MemoryAlloctor<128, 10> _pMem2;
	MemoryAlloctor<256, 10> _pMem3;
	MemoryAlloctor<512, 10> _pMem4;
	MemoryAlloctor<1024, 10> _pMem5;

	MemoryAlloc* _szAlloc[MAX_MEMORY_SZIE + 1];
};