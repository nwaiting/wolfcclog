#ifndef  __FIXMEMPOOL_H__
#define  __FIXMEMPOOL_H__

/*
������������new,delete
�����װ��һ���ڴ�أ�����ģ���࣬
ģ����������ڴ���з���Ķ�������
������Ҫ��������ʵ�֣������ڹ̶���С���ڴ�����
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

    _MemBlock* m_pBlockHeader;		//���ڴ������ͷָ��
    _MemNode*  m_FreeNodeHeader;	//���е�С��ͷָ��
    int m_BlockSize;								//��ǰС�ڴ������
    CThreadLock m_lock;									//�ڴ�ص���

private:
    void AllocBlocks();   //�����ڴ��
    void ReallocBlocks(); //�����ڴ�ش�С

public:
    //���캯��,nInitSizeΪ��ʼ�ڴ����
    CFixMemPool(int nInitSize = 1)
        : m_BlockSize(nInitSize), m_pBlockHeader(NULL), m_FreeNodeHeader(NULL)
    {
        AllocBlocks();
    }

    //�����������ͷ����еײ��ڴ��
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

    /*  ����������������Ҫ�Ķ���ӿ� */
    void* AllocBlock(size_t size);                 //���ڴ�������ڴ�,sizeΪ������ڴ���С
    void  FreeBlock(void* pobj, size_t size);    //���ڴ�黹���ڴ�أ�pobj���ڴ��ָ�룬sizeΪ�ڴ���С

    //��ӡ��Ϣ�����ڵ��Գ���
    void PrintTestInfo()
    {
        printf("Handle pool: size = %d\n", m_BlockSize);
    }

};

template<typename T>
void* CFixMemPool<T>::AllocBlock(size_t size)
{
    return malloc(size);
    if (size != sizeof(T)) //����size������T���ʹ�С����ʹ��ȫ�ַ���������ڴ�
    {
        return malloc(size);
    }

    CAutoLock locker(m_lock);

    if (m_FreeNodeHeader == NULL) //��ǰû�п��е��ڴ�飬�������ڴ�
    {
        ReallocBlocks();
    }

    void* p = m_FreeNodeHeader;    //�ӿ��п��з���һ���ڴ�
    m_FreeNodeHeader = m_FreeNodeHeader->pPrev;

    return p;
}

template<typename T>
void  CFixMemPool<T>::FreeBlock(void* pobj, size_t size)
{
    if (pobj == NULL)     //pobj����ΪNULL
        return;
    return free(pobj);
    if (size != sizeof(T))   //����size������T���ʹ�С����ʹ��ȫ���ͷ��ڴ�������ͷ��ڴ�      
    {
        free(pobj);
    }
    else  //���ڴ�黹���ڴ��
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
    //����m_BlockSize��С���ڴ�飬�����ڴ��
    _MemBlock* newBlock = (_MemBlock*)malloc(sizeof(_MemBlock));

    //�������ڴ�
    newBlock->pNode = (_MemNode*)malloc(sizeof(_MemNode)* m_BlockSize);

    //������Ĵ���ڴ�ֳ�С�飬�����ڿ��п�������
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
    //���ڴ������2��
    m_BlockSize *= 2;
    AllocBlocks();
}

//�̶���С����Ӵ���̳оͿ���ʹ������ڴ��
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

