#include "abstractthreadedserver.h"

using namespace jsonrpc;
using namespace std;

// threadPool(threads)：调用线程池的构造函数，实现，
// 创建线程池，并让线程池中的工作线程（消费者线程），从任务队列中，取出一个主线程（生产者线程）放入到任务队列中的任务，并执行这个任务
AbstractThreadedServer::AbstractThreadedServer(size_t threads)
    : running(false), threadPool(threads), threads(threads) {}

AbstractThreadedServer::~AbstractThreadedServer()
{
    this->StopListening();
}

bool AbstractThreadedServer::StartListening()
{
    if (this->running)
    {
        return false;
    }

    if (!this->InitializeListener())
    {
        return false;
    }

    this->running = true;
    // 创建一个主线程（生产者线程）
    this->listenerThread = unique_ptr<thread>(new thread(&AbstractThreadedServer::ListenLoop, this));

    return true;
}

bool AbstractThreadedServer::StopListening()
{
    if (!this->running)
    {
        return false;
    }

    this->running = false;

    this->listenerThread->join();
    return true;
}

// 主线程（生产者线程）执行的代码
void AbstractThreadedServer::ListenLoop()
{
    while (this->running)
    {
        int conn = this->CheckForConnection();

        if (conn > 0)
        {
            // 线程池中线程的个数大于0
            if (this->threads > 0)
            {
                // &AbstractThreadedServer::HandleConnection这个纯虚函数，是：模板方法设计模式中，实现稳定的算法骨架AbstractThreadedServer::ListenLoop()的，步骤函数，
                // 需要分别在子类class UnixDomainSocketServer、class LinuxTcpSocketServer和class FileDescriptorServer中进行实现
                // 此处调用的也是，子类class UnixDomainSocketServer、class LinuxTcpSocketServer和class FileDescriptorServer中实现的函数
                // =============================================================================================================================================
                // 主线程向任务队列tasks中，放入一个任务AbstractThreadedServer::HandleConnection
                // 线程池中的所有线程，执行的都是同一个函数：AbstractThreadedServer::HandleConnection
                // 并从线程池中，唤醒一个工作线程，执行这个任务
                this->threadPool.enqueue(&AbstractThreadedServer::HandleConnection, this, conn);
            }
            else
            {
                // 线程池中的线程的个数为0，即：没有工作线程，
                // 只能由主线程（生产者线程）自己，执行this->HandleConnection(conn);这个，原本该由工作线程执行的任务
                // this->HandleConnection(conn)这个纯虚函数，是：模板方法设计模式中，实现稳定的算法骨架AbstractThreadedServer::ListenLoop()的，步骤函数，
                // 需要分别在子类class UnixDomainSocketServer、class LinuxTcpSocketServer和class FileDescriptorServer中进行实现
                // 此处调用的也是，子类class UnixDomainSocketServer、class LinuxTcpSocketServer和class FileDescriptorServer中实现的函数
                this->HandleConnection(conn);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
