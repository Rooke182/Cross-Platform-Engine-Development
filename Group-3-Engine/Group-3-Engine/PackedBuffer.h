#pragma once

#include <array>
#include <vector>
#include <functional>

typedef long long unsigned int u64;
typedef unsigned int u32;
typedef unsigned char u8;

template <int SIZE>
class PackedBuffer
{
protected:
	std::vector<void*> m_allocated;

public:
	PackedBuffer();

	void* Allocate(size_t size, size_t alignment);
	virtual void Deallocate(void* ptr);

	void Iterate(std::function<void(void*)> func);
};

template<int SIZE>
inline PackedBuffer<SIZE>::PackedBuffer() :
	m_allocated()
{
}

template<int SIZE>
inline void* PackedBuffer<SIZE>::Allocate(size_t size, size_t alignment)
{
	void* mem = malloc(size);
	m_allocated.push_back(mem);
	return mem;
}

template<int SIZE>
inline void PackedBuffer<SIZE>::Deallocate(void* ptr)
{
	for (int i = 0; i < m_allocated.size(); i++)
	{
		if (m_allocated[i] == ptr)
		{
			m_allocated.erase(m_allocated.begin() + i);
			break;
		}
	}

	free(ptr);
}

template<int SIZE>
inline void PackedBuffer<SIZE>::Iterate(std::function<void(void*)> func)
{
	for (int i = 0; i < m_allocated.size(); i++)
	{
		func(m_allocated[i]);
	}
}
