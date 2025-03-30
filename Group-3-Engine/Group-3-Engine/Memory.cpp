#include <cstdlib>
#include <assert.h>
#include "Memory.h"

#ifdef _DEBUG
struct Header
{
	int checkValue;
	int size;
};

struct Footer
{
	int checkValue;
	int size;
};

constexpr int cCheckValue = 0x12341234;

void* operator new(size_t size)
{
	size_t newSize = size + sizeof(Header) + sizeof(Footer);
	void* ptr = malloc(newSize);

	Header* header = (Header*)ptr;
	header->checkValue = cCheckValue;
	header->size = size;

	Footer* footer = (Footer*)((long long int)ptr + size + sizeof(Header));
	footer->checkValue = cCheckValue;
	footer->size = size;

	void* pMem = (void*)((long long int)ptr + sizeof(Header));

	return pMem;
}

void operator delete(void* ptr)
{
	Header* header = (Header*)((long long int)ptr - sizeof(Header));
	assert(header->checkValue == cCheckValue);

	Footer* footer = (Footer*)((long long int)ptr + header->size);
	assert(footer->checkValue == cCheckValue);

	free(header);
}
#endif
