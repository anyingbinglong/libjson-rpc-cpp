#include <iostream>

#include "stubclient.h" // �ͻ��˳���ʹ��stubclient.h�ļ��еĿͻ��˴����
#include <jsonrpccpp/client/connectors/httpclient.h>

using namespace jsonrpc;
using namespace std;

int main()
{
    HttpClient httpclient("http://localhost:8383");// ����Http�ͻ���������
    StubClient c(httpclient);// �����ͻ��˳�����
    try
    {
        cout << c.sayHello("Peter") << endl;// ����Զ�̷���sayHello
        c.notifyServer();// ����Զ��֪ͨnotifyServer
    }
    catch (JsonRpcException e)
    {
        cerr << e.what() << endl;
    }
}
