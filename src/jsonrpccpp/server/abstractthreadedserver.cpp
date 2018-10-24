#include "abstractthreadedserver.h"

using namespace jsonrpc;
using namespace std;

// threadPool(threads)�������̳߳صĹ��캯����ʵ�֣�
// �����̳߳أ������̳߳��еĹ����̣߳��������̣߳�������������У�ȡ��һ�����̣߳��������̣߳����뵽��������е����񣬲�ִ���������
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
    // ����һ�����̣߳��������̣߳�
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

// ���̣߳��������̣߳�ִ�еĴ���
void AbstractThreadedServer::ListenLoop()
{
    while (this->running)
    {
        int conn = this->CheckForConnection();

        if (conn > 0)
        {
            // �̳߳����̵߳ĸ�������0
            if (this->threads > 0)
            {
                // &AbstractThreadedServer::HandleConnection������麯�����ǣ�ģ�巽�����ģʽ�У�ʵ���ȶ����㷨�Ǽ�AbstractThreadedServer::ListenLoop()�ģ����躯����
                // ��Ҫ�ֱ�������class UnixDomainSocketServer��class LinuxTcpSocketServer��class FileDescriptorServer�н���ʵ��
                // �˴����õ�Ҳ�ǣ�����class UnixDomainSocketServer��class LinuxTcpSocketServer��class FileDescriptorServer��ʵ�ֵĺ���
                // =============================================================================================================================================
                // ���߳����������tasks�У�����һ������AbstractThreadedServer::HandleConnection
                // �̳߳��е������̣߳�ִ�еĶ���ͬһ��������AbstractThreadedServer::HandleConnection
                // �����̳߳��У�����һ�������̣߳�ִ���������
                this->threadPool.enqueue(&AbstractThreadedServer::HandleConnection, this, conn);
            }
            else
            {
                // �̳߳��е��̵߳ĸ���Ϊ0������û�й����̣߳�
                // ֻ�������̣߳��������̣߳��Լ���ִ��this->HandleConnection(conn);�����ԭ�����ɹ����߳�ִ�е�����
                // this->HandleConnection(conn)������麯�����ǣ�ģ�巽�����ģʽ�У�ʵ���ȶ����㷨�Ǽ�AbstractThreadedServer::ListenLoop()�ģ����躯����
                // ��Ҫ�ֱ�������class UnixDomainSocketServer��class LinuxTcpSocketServer��class FileDescriptorServer�н���ʵ��
                // �˴����õ�Ҳ�ǣ�����class UnixDomainSocketServer��class LinuxTcpSocketServer��class FileDescriptorServer��ʵ�ֵĺ���
                this->HandleConnection(conn);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
