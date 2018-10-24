#ifndef ABSTRACTTHREADEDSERVER_H
#define ABSTRACTTHREADEDSERVER_H

#include "abstractserverconnector.h"
#include "threadpool.h"
#include <memory>
#include <thread>
// 这个类的实现，使用了模板方法设计模式
namespace jsonrpc
{
    class AbstractThreadedServer : public AbstractServerConnector
    {
    public:
        AbstractThreadedServer(size_t threads);
        virtual ~AbstractThreadedServer();        

        // 模板方法设计模式中，稳定的算法骨架，需要在父类class AbstractProtocolHandler （也就是这个类）中进行实现
        virtual bool StartListening();
        // 模板方法设计模式中，稳定的算法骨架，需要在父类class AbstractProtocolHandler （也就是这个类）中进行实现
        virtual bool StopListening();

    protected:
        // 下面3个纯虚函数，是：模板方法设计模式中，实现上面稳定的算法骨架的，步骤函数，
        // 需要分别在子类class UnixDomainSocketServer、class LinuxTcpSocketServer和class FileDescriptorServer中进行实现
        /**
         * @brief InitializeListener should initialize sockets, file descriptors etc.
         * @return
         */
        virtual bool InitializeListener() = 0;

        /**
         * @brief CheckForConnection should poll for a new connection. This must be
         * a non-blocking call.
         * @return a handle which is passed on to HandleConnection()
         */
        virtual int CheckForConnection() = 0;

        /**
         * @brief HandleConnection must handle connection information for a given
         * handle that has been returned by CheckForConnection()
         * @param connection
         */
        virtual void HandleConnection(int connection) = 0;

    private:
        bool running;
        std::unique_ptr<std::thread> listenerThread;
        ThreadPool threadPool;// 线程池
        size_t threads;// 指定：线程池中线程的个数

        void ListenLoop();
    };
}

#endif // ABSTRACTTHREADEDSERVER_H
