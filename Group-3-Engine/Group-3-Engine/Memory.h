#pragma once

#ifdef _DEBUG
void* operator new (size_t size);
void operator delete(void* ptr);
#endif
