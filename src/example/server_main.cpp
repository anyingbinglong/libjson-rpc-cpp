#include <iostream>
#include "abstractstubserver.h"
#include <jsonrpccpp/server/connectors/httpserver.h>

using namespace jsonrpc;
using namespace std;

// �ֶ���д�ģ�
// ����˳���ģ��������˴����
// --- �������Json-Rpc���ʹ���ߣ�ʹ��Json-Rpc�⣬�Զ���ķ���˳���
class MyStubServer : public AbstractStubServer
{
    public:
        MyStubServer(AbstractServerConnector &connector);
        
        // Json-Rpc���ʹ���ߣ�ʹ��Json-Rpc�⣬�Զ���ķ���˳����У�ʵ�ֵ�Զ�̷���
        virtual std::string sayHello(const std::string& name);        
        // Json-Rpc���ʹ���ߣ�ʹ��Json-Rpc�⣬�Զ���ķ���˳����У�ʵ�ֵ�Զ��֪ͨ
        virtual void notifyServer();

};

MyStubServer::MyStubServer(AbstractServerConnector &connector) :
    AbstractStubServer(connector)
{
}

// Json-Rpc���ʹ���ߣ�ʹ��Json-Rpc�⣬�Զ���ķ���˳����У�ʵ�ֵ�Զ�̷���
string MyStubServer::sayHello(const string &name)
{
    return "Hello " + name;
}

// Json-Rpc���ʹ���ߣ�ʹ��Json-Rpc�⣬�Զ���ķ���˳����У�ʵ�ֵ�Զ��֪ͨ
void MyStubServer::notifyServer()
{
    cout << "Server got notified" << endl;
}

int main()
{
    HttpServer httpserver(8383);
    // �ֶ���д�ģ�
	// ����˳���ģ��������˴����
    MyStubServer s(httpserver);
    s.StartListening();
    getchar();
    s.StopListening();
    return 0;
}
