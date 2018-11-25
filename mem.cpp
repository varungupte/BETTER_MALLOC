#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include "mem.h"
#define handle_error(msg) \
       	do { perror(msg);  } while (0)
// #define handle_error(msg) \
//        	do { perror(msg); exit(EXIT_FAILURE); } while (0)       	

using namespace std;	
void *ptr=NULL;

struct mem_node						//main structure of header
{
	void *data;
	int size;
};

struct mem_node *freeHead = NULL;		//head of free list
struct mem_node *allocHead = NULL;		//head of alloc list

int freecount=1;						//number of free header in free list
int alloccount=0;						//number of header in alloc list
int presentspace;	
int sizegarbage=0;					// present available space 

mem_node *freelast=NULL;				// point to last header in free list 
mem_node *alloclast=NULL;				// point to last header in alloc list

int Mem_Init(int sizeOfRegion)			// initializations
{	

    if(sizeOfRegion<=0)
    {	
    	handle_error("invalid size of region");
    	return -1;
    	
    }
    sizegarbage=sizeOfRegion;
    int fd = open("/dev/zero", O_RDWR);   // size (in bytes) needs to be evenly divisible by the page size
    ptr = mmap(NULL, 7*sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); 		// getting memory from OS
    if (ptr == MAP_FAILED) { 
        handle_error("mmap");
        return -1;
        
    } 
    presentspace=sizeOfRegion;
    freeHead=(mem_node*)(ptr + 7*sizeOfRegion - sizeof(mem_node));
    allocHead=(mem_node*)(ptr + sizeOfRegion);
    allocHead->size=0;
    freeHead->data=ptr;
    freeHead->size=sizeOfRegion; //initially complete memory allocated is free
    
    freelast=freeHead;
    alloclast=allocHead;
    close(fd); 
    return 1;    
}
 

void *Mem_Alloc(int size)
{
	if(size>presentspace)			// if free space is less than required
	{
		

		handle_error("not enough memory\n");

		return NULL;
	}	
	
	mem_node *biggestnode=freeHead;
	mem_node *currentnode=biggestnode;
	void *userptr=biggestnode->data;			// pointer which will be returned to user
	int biggestsize=biggestnode->size;
	
	for(int i=1;i<=freecount;i++)				// worst fit
	{	
		if(biggestnode->size < currentnode->size)
			biggestnode=currentnode;
		
		currentnode--;
	}
	
	userptr=biggestnode->data;
	mem_node *newallo;
	
	if(size<biggestnode->size)				// partial allocation
	{	
		
		biggestnode->size-=size;			// free header
		biggestnode->data+=size;			// free header
		
		if(allocHead->size != 0)			// alloc header
			newallo=alloclast +1;// sizeof(mem_node);
		
		else
			newallo=allocHead;
		
		newallo->size=size;
		newallo->data=userptr;
		alloccount++;						// increase num of alloc header
		alloclast=newallo;
		presentspace-=size;					// update remaining space
	}
	else if(size==biggestnode->size)		// full allocation
	{
		
		if(allocHead->size != 0)			// alloc header
			newallo=alloclast + 1;
		else
			newallo=allocHead;

		newallo->size=size;
		newallo->data=userptr;
		alloccount++;
		alloclast=newallo;
		presentspace-=size;
		
		if(biggestnode!=freelast)			// free header
		{
			biggestnode->data=freelast->data;
			biggestnode->size=freelast->size;
		}
		
		freelast++;						// increase num of free header
		if(freecount>1)
			freecount--;
		else if(freecount==1)
		{
			freeHead->data=NULL;
			freeHead->size=0;
		}	

		
	}	
	else							// if biggest node is less than size required
	{
		handle_error("not enough memory\n");
		return NULL;
	}	
	return userptr;
}



int Mem_Free(void *ptr1)
{
	if(ptr1==NULL)
		return -1;


	if(!Mem_IsValid(ptr1))				//check whether ptr provided by user is within the valid range
	{	
		handle_error("Not a valid pointer\n");
		return -1;
	}	
	mem_node *currentnode=allocHead;
	unsigned long long int low,high;
	
	for(int i=0;i<alloccount;i++)		//traverse the allocated list to find the particular alloc header
	{
		
		low=(uintptr_t)currentnode->data;
		high=low + currentnode->size;

		if((uintptr_t)low<=(uintptr_t)ptr1 && (uintptr_t)ptr1<(uintptr_t)high)
			break;
		currentnode++;	
	}
	int actual_size=currentnode->size;
	currentnode->size=(uintptr_t)ptr1 - (uintptr_t)low;
	
	mem_node *newfree;
	if(currentnode->size!=1)						//partially freed
	{	
		newfree=freelast;											
		newfree--;
		newfree->size = actual_size - currentnode->size;
	}

	else								//fully freed
	{
		newfree=freelast - 1; 
		newfree->size = actual_size;
		currentnode->size=alloclast->size;
		currentnode->data=alloclast->data;
		alloclast--;
		alloccount--;
	}	
	newfree->data=ptr1;
	freecount++;
	freelast=newfree;
	presentspace+=newfree->size;

	mergeRight((void*)newfree);					//merge 2 adjacent free block into one bigger free block
	mergeLeft((void*)newfree);					//merge 2 adjacent free block into one bigger free block
	return 1;
}

int Mem_GetSize(void *ptr1)
{
    mem_node *currentnode=allocHead;
	unsigned long long int low,high;
	
	for(int i=0;i<alloccount;i++)				//traverse the allocated list to find the particular alloc header
	{
		
		low=(uintptr_t)currentnode->data;
		high=low + currentnode->size;

		if((uintptr_t)low<=(uintptr_t)ptr1 && (uintptr_t)ptr1 < (uintptr_t)high)
		{
			return currentnode->size;			//return size of desired header
		}
		currentnode++;
	}
	return -1;

}

int Mem_IsValid(void *user_ptr)
{
	mem_node *currentnode=allocHead;
	unsigned long long int low,high;			//traverse the allocated list to find the particular alloc header
	for(int i=0;i<alloccount;i++)
	{
		low=(uintptr_t)currentnode->data;
		high=low + currentnode->size;
		if((uintptr_t)low<=(uintptr_t)user_ptr && (uintptr_t)user_ptr<(uintptr_t)high)
			return 1;
		currentnode++;
	}
	return 0;
}

void mergeRight(void *ptr1)
{
	mem_node *ptr=(mem_node*)ptr1;
	int low=(uintptr_t)ptr->data;
	int high=low + ptr->size;
	mem_node *currentnode=freeHead;
	int low1;
	for (int i = 0; i < freecount; ++i)			//traverse the free list
	{
		low1=(uintptr_t)currentnode->data;
		if(high == low1)						// if there is free block just after(right side) the ptr1
		{
			ptr->size+=currentnode->size;		// merge them
			currentnode->size=freelast->size;	// fill the empty free header
			currentnode->data=freelast->data;
			freelast++;
			freecount--;						// decrease free header count
			break;
		}	
		currentnode--;
	}
	return;
}

void mergeLeft(void *ptr1)
{
	mem_node *ptr=(mem_node*)ptr1;
	int low=(uintptr_t)ptr->data;
	int high=low + ptr->size;
	mem_node *currentnode=freeHead;
	int high1;
	for (int i = 0; i < freecount; ++i)			//treverse the free list
	{
		high1=(uintptr_t)currentnode->data + currentnode->size;
		if(high1 == low)						// if there is free block just before(left side) the ptr1
		{
			currentnode->size+=ptr->size;		//merge them
			freelast++;
			freecount--;						// decrease free header count
			break;
		}	
		currentnode--;
	}
	return;
}
void garbage_collector()
{
	sbrk(-sizegarbage);
	cout<<"Memory allocated clear."<<endl;
}





