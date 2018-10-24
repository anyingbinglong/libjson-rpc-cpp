/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    responsehandler.cpp
 * @date    13.03.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include "rpcprotocolclient.h"
#include <jsonrpccpp/common/jsonparser.h>

using namespace jsonrpc;
// �������
const std::string RpcProtocolClient::KEY_PROTOCOL_VERSION = "jsonrpc";
const std::string RpcProtocolClient::KEY_PROCEDURE_NAME = "method";
const std::string RpcProtocolClient::KEY_ID = "id";
const std::string RpcProtocolClient::KEY_PARAMETER = "params";
const std::string RpcProtocolClient::KEY_AUTH = "auth";
// ��Ӧ����
const std::string RpcProtocolClient::KEY_RESULT = "result";
const std::string RpcProtocolClient::KEY_ERROR = "error";
// �������
const std::string RpcProtocolClient::KEY_ERROR_CODE = "code";
const std::string RpcProtocolClient::KEY_ERROR_MESSAGE = "message";
const std::string RpcProtocolClient::KEY_ERROR_DATA = "data";

RpcProtocolClient::RpcProtocolClient(clientVersion_t version)
    : version(version) {}

// �������ܣ�����RPC�������
// �����������壺
//      method���ͻ��˳�������ķ�����������������
//      parameter�������ͻ��˳�������ķ������������Ĳ���
//      result�������õ�json��ʽ��RPC������󣬴�ŵ�����
//      isNotification��֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
//                      �ò��������ڿ����Ƿ񹹽�һ��֪ͨ
void RpcProtocolClient::BuildRequest(const std::string &method,
                                     const Json::Value &parameter,
                                     std::string &result, bool isNotification)
{
    Json::Value request;// �����õ�json��ʽ��RPC������󣬴�ŵ�����
    Json::FastWriter writer;
    // ����ʵʩ����json��ʽ��RPC�������
    this->BuildRequest(1, method, parameter, request, isNotification);
    result = writer.write(request);// ��request��ɲ��Ű��json��ʽ�ַ����������������ϴ��䣬���ٴ���������
}

// �������˷��ظ��ͻ��˵�json��ʽ����Ӧ����
// response������˷��ظ��ͻ��˵���Ӧ����
// result����ŷ���˳��򷵻صĵ��ú�����ִ�н��
void RpcProtocolClient::HandleResponse(const std::string &response, Json::Value &result)
{
    Json::Reader reader;// ʹ��Json::Reader���ظ��ַ���response����ΪJson��ʽ
    Json::Value value;// ʹ��Json::Reader���ظ��ַ���response����ΪJson��ʽ�󣬽����Ľ������ŵ�������
    if (reader.parse(response, value))// ʹ��Json::Reader���ظ��ַ���response����ΪJson��ʽ�������������ŵ�value��
    {
        // ==========================================================================================================
		// ����������������ã����ǶԷ���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���value�����з�����
		// ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
		// ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(value)��
		//      ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
		// ==========================================================================================================
        this->HandleResponse(value, result);
    }
    else
    {
        throw JsonRpcException(Errors::ERROR_RPC_JSON_PARSE_ERROR, " " + response);
    }
}

// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ�����ǰ������·�ʽ���ڷ���˳����н��й���ģ�
// ������һ��rpc����ʱ����֪֮ͨ�⣬����˶�����ظ���Ӧ��
// ��Ӧ��ʾΪһ��JSON����ʹ�����³�Ա��
// jsonrpc
//     ָ��JSON-RPCЭ��汾���ַ���������׼ȷдΪ��2.0��
// result
//     �ó�Ա��rpc���óɹ�ʱ������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����б�������ó�Ա��
//     �����÷���˳��򷽷��������ʱ������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����в����԰����ó�Ա��
//     ������еı����÷��������˸ó�Ա��ֵ��
// error
//     �ó�Ա��rpc����ʧ��ʱ������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����б�������ó�Ա��
//     �ó�Ա��rpc���óɹ�ʱ������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����в����԰����ó�Ա��
//     �ó�Ա�Ĳ���ֵ������Ϊ5.1�ж���Ĵ������
// id
//     �ó�Ա���������
//     �ó�Աֵ��������������е�id��Աֵһ�¡�
//     ����������������У�����������Ϣ����������������Ч���󣩣����ֵ����Ϊ��ֵ��
//     ����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����У��������result��error��Ա����������Ա���ܱ�ͬʱ������
// 5.1�������
//     ��һ��rpc������������ʱ�����ص���Ӧ���������������Ա����������Ϊ�������г�Ա�����Ķ���
// code
//     ʹ����ֵ��ʾ���쳣�Ĵ������͡� ����Ϊ������
// message
//     �Ըô���ļ������ַ����� ������Ӧ�����޶��ڼ�̵�һ�仰��
// data
//     �������ڴ��󸽼���Ϣ�Ļ������ͻ�ṹ�����͡��ó�Ա�ɺ��ԡ� �ó�Աֵ�ɷ���˶��壨������ϸ�Ĵ�����Ϣ��Ƕ�׵Ĵ���ȣ���
// ==========================================================================================================
// ����������������ã����ǶԷ���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���value�����з�����
// ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
// ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(value)��
//      ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
// ==========================================================================================================
Json::Value RpcProtocolClient::HandleResponse(const Json::Value &value, Json::Value &result)
{
    // �жϣ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���value����һ����ȷ�����ã���json��ʽ����Ӧ���󣨻ظ���
    if (this->ValidateResponse(value))// ����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���value��
    {                                 // ��һ����ȷ�����ã���json��ʽ����Ӧ���󣨻ظ���
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ�������С�error����Ա������:
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // ��ζ�ţ��ͻ��˳��������˳��򣬷���ģ�rpc����ʧ��
        if (this->HasError(value))      {
            this->throwErrorException(value);
        }
        else// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ������û�С�error����Ա�����С�result����Ա
        {   // ��ζ�ţ��ͻ��˳��������˳��򣬷���ģ�rpc���óɹ�
            result = value[KEY_RESULT];
        }
    }
    else
    {
        throw JsonRpcException(Errors::ERROR_CLIENT_INVALID_RESPONSE,
                               " " + value.toStyledString());
    }
    return value[KEY_ID];
}

// �������ܣ�����ʵʩ����json��ʽ��RPC�������
// �����������壺
//      method���ͻ��˳�������ķ�����������������
//      parameter�������ͻ��˳�������ķ������������Ĳ���
//      result�������õ�json��ʽ��RPC������󣬴�ŵ�����
//      isNotification��֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
//                      �ò��������ڿ����Ƿ񹹽�һ��֪ͨ
void RpcProtocolClient::BuildRequest(int id, const std::string &method,
                                     const Json::Value &parameter,
                                     Json::Value &result, bool isNotification)
{
    if (this->version == JSONRPC_CLIENT_V2)
    {
        result[KEY_PROTOCOL_VERSION] = "2.0";
    }
    result[KEY_PROCEDURE_NAME] = method;
    if (parameter != Json::nullValue)
    {
        result[KEY_PARAMETER] = parameter;
    }
    // ֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
    if (!isNotification)// ����֪ͨ����ζ�ţ���json��ʽ��RPC��������а�����id����Ա
    {
        result[KEY_ID] = id;
    }
    else if (this->version == JSONRPC_CLIENT_V1)
    {
        result[KEY_ID] = Json::nullValue;
    }
}

// ���÷�����в����ڵķ�����rpc����:
// �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
// --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
// <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// =================================================================================================================
// ����������������ã����ǶԷ���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���response�����з�����
// ��1����response�еģ�"error": {"code": -32601, "message": "Method not found"��"data": }�����ݣ���Ϊ�쳣��Ϣ���׳�
// ��2����response�еģ�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
// ��3����response�еģ�"error": {"code": -32601}�����ݣ���Ϊ�쳣��Ϣ���׳�
// =================================================================================================================
void RpcProtocolClient::throwErrorException(const Json::Value &response)
{
    if (response[KEY_ERROR].isMember(KEY_ERROR_MESSAGE) &&
            response[KEY_ERROR][KEY_ERROR_MESSAGE].isString())
    {
        if (response[KEY_ERROR].isMember(KEY_ERROR_DATA))
        {
            throw JsonRpcException(response[KEY_ERROR][KEY_ERROR_CODE].asInt(),
                                   response[KEY_ERROR][KEY_ERROR_MESSAGE].asString(),
                                   response[KEY_ERROR][KEY_ERROR_DATA]);
        }
        else
        {
            throw JsonRpcException(response[KEY_ERROR][KEY_ERROR_CODE].asInt(),
                                   response[KEY_ERROR][KEY_ERROR_MESSAGE].asString());
        }
    }
    else
    {
        throw JsonRpcException(response[KEY_ERROR][KEY_ERROR_CODE].asInt());
    }
}

// �жϣ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ�������һ����ȷ�����ã���json��ʽ����Ӧ���󣨻ظ���
bool RpcProtocolClient::ValidateResponse(const Json::Value &response)
{
    if (!response.isObject() || !response.isMember(KEY_ID))
    {
        return false;
    }

    if (this->version == JSONRPC_CLIENT_V1)
    {
        if (!response.isMember(KEY_RESULT) || !response.isMember(KEY_ERROR))
        {
            return false;
        }
        if (!response[KEY_RESULT].isNull() && !response[KEY_ERROR].isNull())
        {
            return false;
        }
        if (!response[KEY_ERROR].isNull() &&
                !(response[KEY_ERROR].isObject() &&
                  response[KEY_ERROR].isMember(KEY_ERROR_CODE) &&
                  response[KEY_ERROR][KEY_ERROR_CODE].isIntegral()))
        {
            return false;
        }

    }
    else if (this->version == JSONRPC_CLIENT_V2)
    {
        if (!response.isMember(KEY_PROTOCOL_VERSION) ||
                response[KEY_PROTOCOL_VERSION] != "2.0")
        {
            return false;
        }

        if (response.isMember(KEY_RESULT) && response.isMember(KEY_ERROR))
        {
            return false;
        }
        if (!response.isMember(KEY_RESULT) && !response.isMember(KEY_ERROR))
        {
            return false;
        }
        if (response.isMember(KEY_ERROR) &&
                !(response[KEY_ERROR].isObject() &&
                  response[KEY_ERROR].isMember(KEY_ERROR_CODE) &&
                  response[KEY_ERROR][KEY_ERROR_CODE].isIntegral()))
        {
            return false;
        }

    }

    return true;
}

// �жϣ�
// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ�����У��Ƿ��С�error����Ա������:
// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
bool RpcProtocolClient::HasError(const Json::Value &response)
{
    if (this->version == JSONRPC_CLIENT_V1 && !response[KEY_ERROR].isNull())
    {
        return true;
    }
    else if (this->version == JSONRPC_CLIENT_V2 && response.isMember(KEY_ERROR))
    {
        return true;
    }
    return false;
}
