#include "Alloctor.h"
#include<iostream>
#include"MemoryMgr.hpp"

//void* operator new(size_t size) {
//	//return malloc(size);
//	void* pReturn = MemoryMgr::Instance().allMemory(size);
//	//std::cout << "pReturn" << pReturn << std::endl;
//	return pReturn;
//}

void operator delete(void* p) {
	free(p);
}
//
//void* operator new[](size_t size) {
//	//return malloc(size);
//	return  MemoryMgr::Instance().allMemory(size);
//}

void operator delete[](void* p) {
	free(p);
}

void* mem_alloc(size_t size) {
	return malloc(size);
}

void mem_free(void* p) {
	free(p);
}

