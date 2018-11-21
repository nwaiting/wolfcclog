#ifndef  __FIXMEMPOOL_H__
#define  __FIXMEMPOOL_H__

/*
此类用于重载new,delete
本类封装了一个内存池，采用模板类，
模板参数就是内存池中分配的对象类型
本类主要用链表来实现，适用于固定大小的内存块分配
*/
#include <stdlib.h>
#include "Lock.h"
#include <stddef.h>
#include "general_alloctor.h"
template<typename T>
class CFixMemPool
{

    struct _MemNode
    {
        _MemNode* pPrev;
        char data[sizeof(T) - sizeof(_MemNode*)];
    };

    struct _MemBlock
    {
        _MemBlock* pPrev;
        _MemNode* pNode;
    };

    _MemBlock* m_pBlockHeader;		//大内存块链表头指针
    _MemNode*  m_FreeNodeHeader;	//空闲的小块头指针
    int m_BlockSize;								//当前小内存块数量
    CThreadLock m_lock;									//内存池的锁

private:
    void AllocBlocks();   //分配内存块
    void ReallocBlocks(); //扩大内存池大小

public:
    //构造函数,nInitSize为初始内存块数
    CFixMemPool(int nInitSize = 1)
        : m_BlockSize(nInitSize), m_pBlockHeader(NULL), m_FreeNodeHeader(NULL)
    {
        AllocBlocks();
    }

    //析构函数，释放所有底层内存块
    ~CFixMemPool()
    {
        while (m_pBlockHeader != NULL)
        {
            _MemBlock* tmp = m_pBlockHeader;
            m_pBlockHeader = tmp->pPrev;
            free(tmp->pNode);
            free(tmp);
        }
    }

    /*  下面两个函数是主要的对外接口 */
    void* AllocBlock(size_t size);                 //从内存池请求内存,size为分配的内存块大小
    void  FreeBlock(void* pobj, size_t size);    //把内存归还给内存池，pobj是内存块指针，size为内存块大小

    //打印信息，用于调试程序
    void PrintTestInfo()
    {
        printf("Handle pool: size = %d\n", m_BlockSize);
    }

};

template<typename T>
void* CFixMemPool<T>::AllocBlock(size_t size)
{
    return malloc(size);
    if (size != sizeof(T)) //假如size不等于T类型大小，则使用全局分配符分配内存
    {
        return malloc(size);
    }

    CAutoLock locker(m_lock);

    if (m_FreeNodeHeader == NULL) //当前没有空闲的内存块，则扩大内存
    {
        ReallocBlocks();
    }

    void* p = m_FreeNodeHeader;    //从空闲块中分配一块内存
    m_FreeNodeHeader = m_FreeNodeHeader->pPrev;

    return p;
}

template<typename T>
void  CFixMemPool<T>::FreeBlock(void* pobj, size_t size)
{
    if (pobj == NULL)     //pobj不能为NULL
        return;
    return free(pobj);
    if (size != sizeof(T))   //假如size不等于T类型大小，则使用全局释放内存操作符释放内存      
    {
        free(pobj);
    }
    else  //将内存归还给内存池
    {
        CAutoLock locker(m_lock);
        _MemNode* p = (_MemNode*)pobj;
        p->pPrev = m_FreeNodeHeader;
        m_FreeNodeHeader = p;
    }
}

template<typename T>
void CFixMemPool<T>::AllocBlocks()
{
    //分配m_BlockSize大小个内存块，放入内存池
    _MemBlock* newBlock = (_MemBlock*)malloc(sizeof(_MemBlock));

    //分配大块内存
    newBlock->pNode = (_MemNode*)malloc(sizeof(_MemNode)* m_BlockSize);

    //将分配的大块内存分成小块，串接在空闲块链表上
    newBlock->pNode->pPrev = NULL;
    for (int i = 1; i < m_BlockSize; ++i)
    {
        newBlock->pNode[i].pPrev = &(newBlock->pNode[i - 1]);
    }
    m_FreeNodeHeader = &newBlock->pNode[m_BlockSize - 1];

    newBlock->pPrev = m_pBlockHeader;
    m_pBlockHeader = newBlock;
}

template<typename T>
void CFixMemPool<T>::ReallocBlocks()
{
    //将内存池扩大2倍
    m_BlockSize *= 2;
    AllocBlocks();
}

//固定大小对象从此类继承就可以使用这个内存池
template<class obj>
struct  FixObject
{
    static void* operator new (size_t size)
    {
#ifdef __MY_Win_32__
        return memPool.AllocBlock(size);
#else
        return ned_alloc::allocate(size);
#endif

    }

        static void operator delete (void * p, size_t size)
    {
#ifdef __MY_Win_32__
        memPool.FreeBlock(p, size);
#else
        ned_alloc::deallocate(p, size);
#endif
    }

    static CFixMemPool<obj> memPool;
};

template<class obj>
CFixMemPool<obj> FixObject<obj>::memPool;

#endif

