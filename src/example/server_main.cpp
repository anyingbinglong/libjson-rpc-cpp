#include <iostream>
#include "abstractstubserver.h"
#include <jsonrpccpp/server/connectors/httpserver.h>

using namespace jsonrpc;
using namespace std;

// 手动书写的，
// 服务端程序的，具体服务端存根类
// --- 这个就是Json-Rpc库的使用者，使用Json-Rpc库，自定义的服务端程序
class MyStubServer : public AbstractStubServer
{
    public:
        MyStubServer(AbstractServerConnector &connector);
        
        // Json-Rpc库的使用者，使用Json-Rpc库，自定义的服务端程序中，实现的远程方法
        virtual std::string sayHello(const std::string& name);        
        // Json-Rpc库的使用者，使用Json-Rpc库，自定义的服务端程序中，实现的远程通知
        virtual void notifyServer();

};

MyStubServer::MyStubServer(AbstractServerConnector &connector) :
    AbstractStubServer(connector)
{
}

// Json-Rpc库的使用者，使用Json-Rpc库，自定义的服务端程序中，实现的远程方法
string MyStubServer::sayHello(const string &name)
{
    return "Hello " + name;
}

// Json-Rpc库的使用者，使用Json-Rpc库，自定义的服务端程序中，实现的远程通知
void MyStubServer::notifyServer()
{
    cout << "Server got notified" << endl;
}

int main()
{
    HttpServer httpserver(8383);
    // 手动书写的，
	// 服务端程序的，具体服务端存根类
    MyStubServer s(httpserver);
    s.StartListening();
    getchar();
    s.StopListening();
    return 0;
}
