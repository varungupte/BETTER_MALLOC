#include <bits/stdc++.h>
#include <stdint.h>
#include "mem.h"
using namespace std;

int main()
{
	if(!Mem_Init(8))
	{
		cout<<"init failed";
		return 1;
	}
	void *ptr=Mem_Alloc(4);
	//Mem_Free(ptr+20);
	Mem_Free(ptr);
	void *ptr1=Mem_Alloc(4);
	// cout<<"hello";
	// void *ptr2=Mem_Alloc(1);
	// cout<<"hi";
	// Mem_Free(ptr1+3);
	// Mem_Free(ptr1+1);
	// ptr2=Mem_Alloc(5);
	// Mem_Free(ptr+1);
	// cout<<"hi";
	// ptr2=Mem_Alloc(5);
	// cout<<ptr1<<"-----------\n";
	void *ptr2=Mem_Alloc(16);
	cout<<"Is Valid of Ptr2 "<<Mem_IsValid(ptr2+1)<<endl;
	Mem_Free(ptr1+4);
	cout<<"Is Valid of Ptr1+3 "<<Mem_IsValid(ptr1+3)<<endl;
	cout<<"Is Valid of Ptr1+4 "<<Mem_IsValid(ptr1+4)<<endl;
	Mem_Free(ptr2+8);
	cout<<"Is Valid of Ptr2 "<<Mem_IsValid(ptr2+1)<<endl;
	void *ptr3=Mem_Alloc(15);
	Mem_Free(ptr3+8);
	// void *ptr4=Mem_Alloc(16);
	// Mem_Free(ptr4);
	// void *ptr5=Mem_Alloc(16);
	// Mem_Free(ptr5+8);
	//cout<<"Is Valid of Ptr1 "<<Mem_IsValid(ptr1+5)<<endl;
	
	cout<<"Is Valid of Ptr2+3 "<<Mem_IsValid(ptr2+3)<<endl;
	cout<<"Is Valid of Ptr2+4 "<<Mem_IsValid(ptr2+4)<<endl;
	cout<<"Is Valid of Ptr2+5 "<<Mem_IsValid(ptr2+5)<<endl;
	garbage_collector();
	return 0;
}