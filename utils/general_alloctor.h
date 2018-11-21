#ifndef __GENERAL_ALLOCATOR_H__
#define __GENERAL_ALLOCATOR_H__

#include <stdlib.h>
#include "lock.h"
#ifndef __MY_Win_32__
#include  "nedmalloc.h"
#endif

class GeneralAlloctor {

private:
	// Really we should use static const int x = N
	// instead of enum { x = N }, but few compilers accept the former.
	enum {_ALIGN = 4};
	enum {_MAX_BYTES = 512};
	enum {_NFREELISTS = 128}; // _MAX_BYTES/_ALIGN
	static size_t _S_round_up(size_t __bytes) 
	{ 
		return (((__bytes) + (size_t) _ALIGN-1) & ~((size_t) _ALIGN - 1));
	}

	union _Obj {
		union _Obj* _M_free_list_link;
		char _M_client_data[1];    /* The client sees this.        */
	};

	static _Obj* volatile  _S_free_list[_NFREELISTS]; 

	static  size_t _S_freelist_index(size_t __bytes) 
	{
		return (((__bytes) + (size_t)_ALIGN-1)/(size_t)_ALIGN - 1);
	}

	// Returns an object of size __n, and optionally adds to size __n free list.
	static void* _S_refill(size_t __n);
	// Allocates a chunk for nobjs of size size.  nobjs may be reduced
	// if it is inconvenient to allocate the requested number.
	static char* _S_chunk_alloc(size_t __size, int& __nobjs);

	// Chunk allocation state.
	static char* _S_start_free;
	static char* _S_end_free;
	static size_t _S_heap_size;
	static CThreadLock _locker;
public:

	/* __n must be > 0      */
	static void* allocate(size_t __n)
	{
		void* __ret = 0;

		if(__n == 0)
			return 0;

			
		if (__n > (size_t) _MAX_BYTES) {
			__ret = malloc(__n);
		}
		else {
			_Obj* volatile * __my_free_list
				= _S_free_list + _S_freelist_index(__n);
			// Acquire the lock here with a constructor call.
			// This ensures that it is released in exit or during stack
			// unwinding.
			CAutoLock locker(_locker);
			_Obj* __result = *__my_free_list;
			if (__result == 0)
				__ret = _S_refill(_S_round_up(__n));
			else {
				*__my_free_list = __result -> _M_free_list_link;
				__ret = __result;
			}
		}

		return __ret;
	};

	/* __p may not be 0 */
	static void deallocate(void* __p, size_t __n)
	{
		if(__p == 0 ||__n == 0)
			return;
	
		if (__n > (size_t) _MAX_BYTES)
			free(__p);
		else {
			_Obj* volatile *  __my_free_list
				= _S_free_list + _S_freelist_index(__n);
			_Obj* __q = (_Obj*)__p;

			// acquire lock
			CAutoLock locker(_locker);
			__q -> _M_free_list_link = *__my_free_list;
			*__my_free_list = __q;
			// lock is released here
		}
	}

	static void* reallocate(void* __p, size_t __old_sz, size_t __new_sz);

} ;


//size小的对象从此类继承就可以使用这个通用内存�?
struct  SmallObject
{
	static void* operator new (size_t size)
	{
		return GeneralAlloctor::allocate(size);
	}

	static void operator delete (void * p, size_t size)
	{
		GeneralAlloctor::deallocate(p, size);
	}
};
#ifndef __MY_Win_32__
class ned_alloc
{
	public:
		static void* allocate(size_t size)
		{
			return nedmalloc(size);
		}
		static void deallocate(void *p, size_t size)
		{
			nedfree(p);
		}
};
#endif

#endif
