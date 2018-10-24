/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    client.cpp
* @date    03.01.2013
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#include "client.h"
#include "rpcprotocolclient.h"

using namespace jsonrpc;

Client::Client(IClientConnector &connector, clientVersion_t version)
    : connector(connector)
{
    // RpcProtocolClient(version)�ĺ��壺
    // json-rpc v2.0�ͻ���Э��ʵ�֣�
    // 1).����RPC�������
    // 2).�Է���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���value�����з�����
    //      ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
    //      ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(value)��
    //          ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
    // ��ϸ���ݣ����Բο�rpcprotocolclient.h�ļ����ݣ��������
    this->protocol = new RpcProtocolClient(version);
}

Client::~Client()
{
    delete this->protocol;
}

// �����������壺
//      name���ͻ��˳�������ķ�����������������
//      parameter�������ͻ��˳�������ķ������������Ĳ���
//      result���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
// �������ܣ�����Զ�̷���
//      1).����RPC�������
//      2).�ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ�����󣩣�
//      ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
//      3).�Է���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���response�����з�����
//          ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
//          ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(response)��
//              ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
void Client::CallMethod(const std::string &name, const Json::Value &parameter,
                        Json::Value &result)
{
    std::string request, response;
    // protocol->BuildRequest�������ܣ�����RPC�������
    // �����������壺
    //      name���ͻ��˳�������ķ�����������������
    //      parameter�������ͻ��˳�������ķ������������Ĳ���
    //      request�������õ�json��ʽ��RPC������󣬴�ŵ�����
    //      isNotification��֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
    //                      �ò��������ڿ����Ƿ񹹽�һ��֪ͨ
    // request��һ�����Ű��json��ʽ�ַ���
    protocol->BuildRequest(name, parameter, request, false);

    // connector����ͻ��˳����������������������������RPC��Ϣ�������շ��������򷵻صĴ�����
    // connector.SendRPCMessage�������ܣ�
    // �ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ�����󣩣�
    // ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
    connector.SendRPCMessage(request, response);

    // protocol->HandleResponse�������ܣ�
    // �Է���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���response�����з�����
    // ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
    // ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(response)��
    //      ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
    protocol->HandleResponse(response, result);
}

void Client::CallProcedures(const BatchCall &calls, BatchResponse &result)
{
    std::string request, response;
    // ���ͻ��˳��򣬷��͸�����˳���ģ�RPC�������������������calls����ɲ��Ű��json��ʽ�ַ����������������ϴ��䣬���ٴ���������
    request = calls.toString();
    // connector����ͻ��˳����������������������������RPC��Ϣ�������շ��������򷵻صĴ�����
    // connector.SendRPCMessage�������ܣ�
    // �ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ���������飬
    // ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ��������
    connector.SendRPCMessage(request, response);
    // ʹ��Json::Reader���ظ��ַ���response����ΪJson��ʽ
    Json::Reader reader;
    // ʹ��Json::Reader���ظ��ַ���response����ΪJson��ʽ�󣬽����Ľ������ŵ�������
    Json::Value tmpresult;

    // ʹ��Json::Reader���ظ��ַ���response����ΪJson��ʽ�������������ŵ�tmpresult��
    if (!reader.parse(response, tmpresult) || !tmpresult.isArray())
    {
        throw JsonRpcException(Errors::ERROR_CLIENT_INVALID_RESPONSE,
                               "Array expected.");
    }

    // ����������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ��������tmpresult
    for (unsigned int i = 0; i < tmpresult.size(); i++)
    {
        if (tmpresult[i].isObject())
        {
            Json::Value singleResult;
            try
            {
                // protocol->HandleResponse�������ܣ�
                // �Է���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���tmpresult[i]�����з�����
                // ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�singleResult��
                // ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(tmpresult[i])��
                //     ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
                Json::Value id = this->protocol->HandleResponse(tmpresult[i], singleResult);
                // �����������壺
                //      id�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ�����id��Ա
                //      singleResult�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ����ľ������ݣ���ŵ�������
                //      isError����ʶ�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ����ľ��������У��Ƿ����"error"��Ա
                // �������ܣ�
                //      �����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����һ����Ӧ����
                // ==================================================================================
                // �����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����һ����Ӧ����ʾ����
                // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
                // <-- [
                // {"jsonrpc": "2.0", "result": 7, "id": "1"},
                // {"jsonrpc": "2.0", "result": 19, "id": "2"},
                // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
                // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
                // ]
                // ==================================================================================
                // �������json��ʽ��RPC��Ӧ���������У���������һ����Ӧ����
                // {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
                // ==================================================================================
                // ������ɺ󣬷���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
                // <-- [
                // {"jsonrpc": "2.0", "result": 7, "id": "1"},
                // {"jsonrpc": "2.0", "result": 19, "id": "2"},
                // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
                // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"},
                // {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
                // ]
                // ==================================================================================
                result.addResponse(id, singleResult, false);
            }
            catch (JsonRpcException &ex)
            {
                Json::Value id = -1;
                if (tmpresult[i].isMember("id"))
                {
                    id = tmpresult[i]["id"];
                }
                result.addResponse(id, tmpresult[i]["error"], true);
            }
        }
        else
        {
            throw JsonRpcException(Errors::ERROR_CLIENT_INVALID_RESPONSE,
                                   "Object in Array expected.");
        }
    }
}

BatchResponse Client::CallProcedures(const BatchCall &calls)
{
    BatchResponse result;
    this->CallProcedures(calls, result);
    return result;
}

// �����������壺
//      name���ͻ��˳�������ķ�����������������
//      parameter�������ͻ��˳�������ķ������������Ĳ���
// ��������ֵ�ĺ��壺
//      �ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ��˸÷���ֵ��
// �������ܣ�����Զ�̷���
Json::Value Client::CallMethod(const std::string &name,
                               const Json::Value &parameter)
{
    Json::Value result;
    // �����������壺
    //      name���ͻ��˳�������ķ�����������������
    //      parameter�������ͻ��˳�������ķ������������Ĳ���
    //      result���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
    // �������ܣ�����Զ�̷���
    //      1).����RPC�������
    //      2).�ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ�����󣩣�
    //      ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
    //      3).�Է���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���response�����з�����
    //          ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
    //          ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(response)��
    //              ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
    this->CallMethod(name, parameter, result);
    return result;
}

// �����������壺
//      name���ͻ��˳��������Զ��֪ͨ��������������
//      parameter�������ͻ��˳��������Զ��֪ͨ���������Ĳ���
// �������ܣ�����Զ��֪ͨ
//      1).����RPC�������
//      2).�ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ�����󣩣�
//         ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
void Client::CallNotification(const std::string &name,
                              const Json::Value &parameter)
{
    std::string request, response;
    // protocol->BuildRequest�������ܣ�����RPC�������
    // �����������壺
    //      name���ͻ��˳�������ķ�����������������
    //      parameter�������ͻ��˳�������ķ������������Ĳ���
    //      request�������õ�json��ʽ��RPC������󣬴�ŵ�����
    //      isNotification��֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
    //                      �ò��������ڿ����Ƿ񹹽�һ��֪ͨ
    // request��һ�����Ű��json��ʽ�ַ���
    protocol->BuildRequest(name, parameter, request, true);

    // connector����ͻ��˳����������������������������RPC��Ϣ�������շ��������򷵻صĴ�����
    // connector.SendRPCMessage�������ܣ�
    // �ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ�����󣩣�
    // ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
    connector.SendRPCMessage(request, response);
}
