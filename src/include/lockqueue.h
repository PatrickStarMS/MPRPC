#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
//异步写日志的日志队列
//由于队列不是简单的queue，因此需要改造一下，建立一个专门的class
template<typename T>
class LockQueue
{
public:
    //读写,多个worker日志，往queue中写
 void Push(const T& data)
 {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(data);
    //因为就一个线程读取线程，因此唤醒一个就行
    m_condvariable.notify_one();
 }
 //一个线程读取queue，往日志文件中写
    //  不能返回局部变量的引用
 T Pop()
 {
    // if(m_queue.empty)
    //条件变量需要传入unique_lock
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_queue.empty())  // 防止线程虚假唤醒
    {
      // 队列为空，进入等待,入队跟出队是同一把锁
      m_condvariable.wait(lock);
    }
    T data = m_queue.front();
    m_queue.pop();
    return data;
 }

private:
 std::queue<T> m_queue;
 std::mutex m_mutex;
 std::condition_variable m_condvariable;
};

