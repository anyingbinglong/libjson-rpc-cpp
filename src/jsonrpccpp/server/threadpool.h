/*
Copyright (c) 2012 Jakob Progsch, V谩clav Zeman

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
distribution.
*/

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace jsonrpc {
class ThreadPool {
public:
    ThreadPool(size_t);
    
    // 向任务队列tasks中，放入一个任务
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
private:
    // need to keep track of threads so we can join them
    // 线程池中的线程
    std::vector< std::thread > workers;
    // the task queue
    // 任务队列:
    // 任务队列中的任务，就是线程池中的线程，需要执行的任务
    // 任务，就是一个函数，一个需要使用线程单独上cpu执行的函数
    std::queue< std::function<void()> > tasks;
    
    // 使用生产者和消费者模型
    // synchronization
    // 互斥锁：保护任务队列，使线程池中的多个线程，互斥访问任务队列
    std::mutex queue_mutex;
    // 条件变量：
    std::condition_variable condition;
    bool stop;
};

// the constructor just launches some amount of workers
// 相当于消费者线程的pop操作
inline ThreadPool::ThreadPool(size_t threads)
    :   stop(false)
{
    for(size_t i = 0;i<threads;++i)
    {
        workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    // 保存从任务队列tasks中取出的一个任务
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        // 从任务队列tasks中取出一个任务
                        task = std::move(this->tasks.front());
                        // 删除刚刚从任务队列tasks中取出的任务
                        this->tasks.pop();
                    }
                    // 将刚刚从任务队列tasks中取出的任务task，交付给线程池workers中的一个线程执行
                    task();
                }
            }
        );
    }
}

// add new work item to the pool
// 向任务队列tasks中，放入一个任务
//　相当于生产者线程的push操作
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
    {
        // 回收线程池workers中的线程，即：让这些线程停止执行，并销毁这些线程
        worker.join();
    }
}
}


#endif
