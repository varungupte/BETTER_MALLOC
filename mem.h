int Mem_Init(int sizeOfRegion);
void *Mem_Alloc(int size);
int Mem_Free(void *ptr);
int Mem_IsValid(void *ptr);
int Mem_GetSize(void *ptr);
void mergeRight(void *ptr);
void mergeLeft(void *ptr);
void garbage_collector();