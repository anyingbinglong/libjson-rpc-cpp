/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    batchcall.cpp
* @date    15.10.2013
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#include "batchcall.h"
#include "rpcprotocolclient.h"

using namespace jsonrpc;
using namespace std;

BatchCall::BatchCall() : id(1) {}

// �����������壺
//      methodname���ͻ��˳�������ķ�����������������
//      params�������ͻ��˳�������ķ������������Ĳ���
//      isNotification��֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
//                      �ò��������ڿ����Ƿ񹹽�һ��֪ͨ
// �������ܣ�
//      ��ͻ��˳��򣬷��͸�����˳���ģ�RPC��������������������У�����һ���������
// ==================================================================================
// ��ͻ��˳��򣬷��͸�����˳���ģ�RPC��������������������У�����һ���������ʾ����
// �ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
// --> [
// {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
// {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
// {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
// {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
// ]
// ==================================================================================
// �������json��ʽ��RPC������������У���������һ���������
// {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]}
// ==================================================================================
// ������ɺ󣬿ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
// --> [
// {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
// {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
// {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
// {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
// {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
// ]
// ==================================================================================
int BatchCall::addCall(const string &methodname, const Json::Value &params,
                       bool isNotification)
{
    Json::Value call;// ��ţ������ӵ�һ���������
    // �����������ӵ�һ���������
    call[RpcProtocolClient::KEY_PROTOCOL_VERSION] = "2.0";
    call[RpcProtocolClient::KEY_PROCEDURE_NAME] = methodname;
    call[RpcProtocolClient::KEY_PARAMETER] = params;
    // �����ӵ����������֪ͨ
    if (!isNotification)
    {
        call[RpcProtocolClient::KEY_ID] = this->id++;
    }
    // ��ͻ��˳��򣬷��͸�����˳���ģ�RPC��������������������У�����һ���������
    this->result.append(call);
    
    // �����ӵ����������֪ͨ
    if (isNotification)
    {
        return -1;
    }
    return call[RpcProtocolClient::KEY_ID].asInt();
}

// �����������壺
//      ��1��fast = true�����ͻ��˳��򣬷��͸�����˳���ģ�RPC�������������������this->result����ɲ��Ű��json��ʽ�ַ����������������ϴ��䣬���ٴ���������
//      ��2��fast = false�����ͻ��˳��򣬷��͸�����˳���ģ�RPC�������������������this->result������Ű��json��ʽ�ַ���
// �������ܣ�
//      ��1�����ͻ��˳��򣬷��͸�����˳���ģ�RPC�������������������this->result����ɲ��Ű��json��ʽ�ַ����������������ϴ��䣬���ٴ���������
//      ��2�����ͻ��˳��򣬷��͸�����˳���ģ�RPC�������������������this->result������Ű��json��ʽ�ַ���
string BatchCall::toString(bool fast) const
{
    string result;// ת���õ�json��ʽ��RPC����������飬��ŵ�����
    if (fast)//��1��fast = true��
    {
        Json::FastWriter writer;
        // ��this->result��ɲ��Ű��json��ʽ�ַ����������������ϴ��䣬���ٴ���������
        result = writer.write(this->result);
    }
    else//��2��fast = false
    {
        Json::StyledWriter writer;
        // ���ͻ��˳��򣬷��͸�����˳���ģ�RPC�������������������this->result������Ű��json��ʽ�ַ���
        result = writer.write(this->result);
    }
    return result;
}
