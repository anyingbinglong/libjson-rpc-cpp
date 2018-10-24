#include <iostream>

#include "stubclient.h" // 客户端程序，使用stubclient.h文件中的客户端存根类
#include <jsonrpccpp/client/connectors/httpclient.h>

using namespace jsonrpc;
using namespace std;

int main()
{
    HttpClient httpclient("http://localhost:8383");// 创建Http客户端连接器
    StubClient c(httpclient);// 创建客户端程序存根
    try
    {
        cout << c.sayHello("Peter") << endl;// 调用远程方法sayHello
        c.notifyServer();// 调用远程通知notifyServer
    }
    catch (JsonRpcException e)
    {
        cerr << e.what() << endl;
    }
}
