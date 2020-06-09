#include<iostream>
#include<string.h>
#include"Alloctor.h"

int main() {
	//1
	//char* data1 = new char[64];
	//printf("sizeOf(char[]) = %d",sizeof(data1));
	//delete[] data1;
	//2
	//char* data2 = new char();
	//delete data2;
	//3
	//char* data3 = (char*)malloc(64);
	//free(data3);

	char* data = (char*)newOfSelf(1);
	*data = 'a';
	
	int* test = (int*)newOfSelf(4);
	*test = 2;

	getchar();
	return 0;
}