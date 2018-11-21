#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

#include <queue>
#include "lock.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 消息队列,适用有1个消费者(从队列中读消息)，多个生产者(往队列里面写消息)
////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class MsgPacket>
class CMessageQueue
{
public:
    CMessageQueue(void) :m_curIndex(0)
    {
        //m_lock = new CThreadLock;
    }
    virtual ~CMessageQueue(void)
    {
        Clear();
        //delete m_lock;
    }

public:

    /*
    *@description: 清除所有的消息包
    */
    void Clear()
    {
        CAutoLock autoLock(m_lock);
        std::queue<MsgPacket*> & proQueue = m_queues[m_curIndex % 2];
        while (!proQueue.empty()) {
            delete proQueue.front();
            proQueue.pop();
        }

        std::queue<MsgPacket*> & conQueue = m_queues[(m_curIndex + 1) % 2];
        while (!conQueue.empty()) {
            delete conQueue.front();
            conQueue.pop();
        }
    }

    size_t GetSize()
    {
        CAutoLock autoLock(m_lock);
        return m_queues[1].size() + m_queues[0].size();
    }

    /*
    *description: 生产一个消息
    */
    void AddMsg(MsgPacket* msg)
    {
        CAutoLock autoLock(m_lock);
        std::queue<MsgPacket*> & proQueue = m_queues[m_curIndex % 2];
        proQueue.push(msg);
    }

    /*
    *description: 消费一个消息
    */

    MsgPacket* PopMsg()
    {
        std::queue<MsgPacket*> & conQueue = GetConQueue();
        if (!conQueue.empty()) {
            MsgPacket* packet = conQueue.front();
            conQueue.pop();
            return packet;
        }
        return NULL;
    }

    /*
    *@description: 返回队列头元素，但是不删除这个元素
    */
    MsgPacket* Front()
    {
        std::queue<MsgPacket*> & conQueue = GetConQueue();
        if (!conQueue.empty()) {
            MsgPacket* packet = conQueue.front();
            return packet;
        }
        return NULL;
    }

    std::queue<MsgPacket*> & GetConQueue()
    {
        std::queue<MsgPacket*> & conQueue = m_queues[(m_curIndex + 1) % 2];
        if (conQueue.empty()) {
            CAutoLock autoLock(m_lock);
            m_curIndex++;
            std::queue<MsgPacket*> & cQueue = m_queues[(m_curIndex + 1) % 2];
            return cQueue;
        }
        return conQueue;
    }

private:
    std::queue<MsgPacket*> m_queues[2]; //一个消费者队列，一个生产者队列
    unsigned int  m_curIndex; //当前生产者用的队列索引
    CThreadLock m_lock;
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 另外一种消息队列,适用有多个消费者(从队列中读消息)，多个生产者
// 生产者共用一个队列，这个队列一般为单件，消费者线程独有自己的队列，消费者从生产者中拷贝消息到自己的队列中执行
////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class MsgPacket>
class CMessageQueueMul
{
public:
    CMessageQueueMul(void)
    {
    }
    virtual ~CMessageQueueMul(void)
    {
        Clear();
    }

public:

    /*
    *@description: 清除所有的消息包
    */
    void Clear()
    {
        CAutoLock autoLock(m_lock);
        while (!m_queue.empty())
        {
            delete m_queue.front();
            m_queue.pop();
        }
    }

    /*
    *description: 生产一个消息
    *parameter: msg要加入队列的消息
    *return: void
    *time: 2009/07/18 by owen
    */
    void AddMsg(MsgPacket* msg)
    {
        CAutoLock autoLock(m_lock);
        if (msg)
            m_queue.push(msg);
    }
    /*
    *description: 拷贝队列中的消息到消费者线程中
    *parameter: queue 消费者线程中的队列
    *return: void
    */
    void ExportMsg(CMessageQueueMul<MsgPacket>& queue)
    {
        CAutoLock alock(m_lock);
        while (!m_queue.empty())
        {
            MsgPacket* packet = m_queue.front();
            if (packet != NULL)
                queue.AddMsg(packet);
            m_queue.pop();
        }
    }

    /*
    *description: 消费一个消息
    *parameter: void
    *return: 返回消息
    *time: 2009/07/18 by owen
    */

    MsgPacket* PopMsg()
    {
        CAutoLock alock(m_lock);
        if (!m_queue.empty())
        {
            MsgPacket* packet = m_queue.front();
            m_queue.pop();
            return packet;
        }
        return NULL;
    }

    /*
    *@description: 返回队列头元素，但是不删除这个元素
    */
    MsgPacket* Front()
    {
        CAutoLock alock(m_lock);
        if (!m_queue.empty())
        {
            MsgPacket* packet = m_queue.front();
            return packet;
        }
        return NULL;
    }

    /*
    *@description: 得到队列中消息数量
    */
    size_t GetSize()
    {
        CAutoLock alock(m_lock);
        return m_queue.size();
    }

    bool Empty()
    {
        CAutoLock alock(m_lock);
        return m_queue.empty();
    }

private:
    std::queue<MsgPacket*> m_queue; //队列
    CThreadLock m_lock;
};

#endif
