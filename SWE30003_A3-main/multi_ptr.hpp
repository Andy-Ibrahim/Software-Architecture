#ifndef _103575527SWE300003A3_MULTIPTR
#define _103575527SWE300003A3_MULTIPTR 1

#include <initializer_list>
#include <cstdlib>
#include <cstring>

typedef void** multi_ptr;

inline multi_ptr mp_new(std::initializer_list<void*> ptrs) { size_t size = ptrs.size() * sizeof(void*); return (multi_ptr) memcpy(malloc(size), ptrs.begin(), size); }
template<typename T> inline T *mp_get(multi_ptr mp, size_t idx) { return (T*) mp[idx]; }

#endif