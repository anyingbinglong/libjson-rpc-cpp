/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    rpcprotocolserverv2.cpp
* @date    31.12.2012
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#include "rpcprotocolserverv2.h"
#include <iostream>
#include <jsonrpccpp/common/errors.h>

using namespace std;
using namespace jsonrpc;

RpcProtocolServerV2::RpcProtocolServerV2(IProcedureInvokationHandler &handler)
    : AbstractProtocolHandler(handler) {}

// �����������壺
//      req����1���ͻ��˳��������˳����͵�req�ǣ�������������
//               �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�����������
//           ��2���ͻ��˳��������˳����͵�req�ǣ���ͨ��������
//               �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�������
//      response����1���ͻ��˳��������˳����͵��ǣ���ͨ��������
//                    ����˳��򷵻ظ��ͻ��˵�response�ǣ�json��ʽ����Ӧ��������
//                ��2���ͻ��˳��������˳����͵��ǣ���ͨ��������
//                    ����˳��򷵻ظ��ͻ��˵�response�ǣ�json��ʽ����Ӧ����
// �������ܣ�
// ʵ�֣���json-rpc v2.0Э���У��ͻ�����ͨ��������Ĵ���
//      �Կͻ��˳��������˳����͵�req -- json��ʽ��RPC���������з����봦��
//      ��������Ľ�� -- json��ʽ����Ӧ���󣬷ŵ�response��
// ʵ�֣���json-rpc v2.0Э���У��ͻ���������������Ĵ���
//      �Կͻ��˳��������˳����͵�req -- json��ʽ��RPC�������������з����봦��
//      ��������Ľ�� -- json��ʽ����Ӧ�������飬�ŵ�response��
void RpcProtocolServerV2::HandleJsonRequest(const Json::Value &req, Json::Value &response)
{
    // It could be a Batch Request
    if (req.isArray())// �ͻ��˳��������˳����͵�req�ǣ�������������
    {
        // �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�����������
        // ʵ�֣���json-rpc v2.0Э���У��ͻ���������������Ĵ���
        // ʲô���������ã�
        // ����Ҫͬʱ���Ͷ���������ʱ���ͻ��˿��Է���һ���������������������顣
        // ���������õ�����������������ʱ�����������Ҫ����һ��������ͻ��˷��͵���������������Ӧ����Ӧ�������顣
        // ��Ӧ���������е�ÿ����Ӧ������������������е�ÿ���������һһ��Ӧ��������֪ͨ���͵��������
        // ����˿��Բ����ģ�������˳��������ȵĲ����ԣ���������Щ�������á�
        // ���ڣ���Щ���Ӧ����Ӧ���󣬿����������˳�򣬰����ڷ��ص���Ӧ���������У����ԣ��ͻ�����Ҫʹ����Ӧ���������и�����Ӧ�����е�id��Ա����ƥ����֮��Ӧ���������
        // ��������ʾ����
        // �ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
        // --> [
        // {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
        // {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
        // {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
        // {"foo": "boo"},
        // {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
        // {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
        // ]
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "result": 19, "id": "2"},
        // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"},
        // {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
        // ]
        this->HandleBatchRequest(req, response);
    } // It could be a simple Request
    else if (req.isObject())// �ͻ��˳��������˳����͵�req�ǣ���ͨ��������
    {
        // �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�������
        // ʵ�֣���json-rpc v2.0Э���У��ͻ�����ͨ��������Ĵ���
        // Json-Rpc V2.0 Э����ͨԶ�̹��̵���ʾ�����£�
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
        // ����˸��ͻ��˷��صģ�json��ʽ����Ӧ�������£�
        // <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
        this->HandleSingleRequest(req, response);
    }
    else
    {
        this->WrapError(Json::nullValue, Errors::ERROR_RPC_INVALID_REQUEST,
                        Errors::GetErrorMessage(Errors::ERROR_RPC_INVALID_REQUEST),
                        response);
    }
}

// �����������壺
//      req���ͻ��˳��������˳����͵�req�ǣ���ͨ��������
//           �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�������
//      response���ͻ��˳��������˳����͵��ǣ���ͨ��������
//                ����˳��򷵻ظ��ͻ��˵�response�ǣ�json��ʽ����Ӧ����
// �������ܣ�
// ʵ�֣���json-rpc v2.0Э���У��ͻ�����ͨ��������Ĵ���
// Json-Rpc V2.0 Э����ͨԶ�̹��̵���ʾ�����£�
// �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
// --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
// ����˸��ͻ��˷��صģ�json��ʽ����Ӧ�������£�
// <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
void RpcProtocolServerV2::HandleSingleRequest(const Json::Value &req, Json::Value &response)
{
    // �����������壺
    //      request���ͻ��˳��������˳����͵�json��ʽ��RPC����������飩
    // ===============================================================================================
    // ��������
    // Validate��ȷ�ϣ�֤ʵ
    // ����ͨ���У��ڽ��յ���ȷ��Ϣ����ȷ��Ϣ��������ͷ����͵�һ���ź�
    // ����˳���ȷ�ϣ���飩���ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request�У������ĸ����ֶ�ֵ��
    // �Ƿ�Ϸ�����������Json-Rpc v2.0Э��Ĺ淶��
    // ===============================================================================================
    // ��������������� class RpcProtocolServerV2 ��ģ�����class AbstractProtocolHandler�еĳ�Ա����
    // ===============================================================================================
    int error = this->ValidateRequest(req);
    if (error == 0)
    {
        try
        {
            // �����������壺
            //      req���ͻ��˳��������˳����͵�json��ʽ��RPC�������
            //      response���Կͻ��˳��������˳����͵�request
            //                -- json��ʽ��RPC���������з����봦��Ľ�� -- json��ʽ����Ӧ���󣬷ŵ�response��
            // =============================================================================================================
            // ��������
            // ʵ�֣���json-rpc v2.0Э���У��ͻ���RPCԶ�̹��̵�������Ĵ���
            // �Կͻ��˳��������˳����͵�request -- json��ʽ��RPC���������з����봦��
            // ��������Ľ�� -- json��ʽ����Ӧ���󣬷ŵ�response��
            // =============================================================================================================
            // ��������������� class RpcProtocolServerV2 ��ģ�����class AbstractProtocolHandler�еĳ�Ա����
            // =============================================================================================================
            this->ProcessRequest(req, response);
        }
        catch (const JsonRpcException &exc)
        {
            // Wrap������
            // ���÷�����в����ڵķ�����rpc����:
            // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
            // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
            // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
            // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
            // ========================================================================================================================
            // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result������Ľ������ŵ�result��
            // ʹ��request��exception���й��죬����Ľ������ŵ�result��
            // ��1����һ�ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
            // ��2���ڶ��ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
            // ========================================================================================================================
            this->WrapException(req, exc, response);
        }
    }
    else
    {
        // Wrap������
        // ���÷�����в����ڵķ�����rpc����:
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
        // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // =================================================================================================================
        // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result������Ľ������ŵ�result��
        // ʹ��request, code��message���й��죬����Ľ������ŵ�result��
        // ��1����һ�ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // ��2���ڶ��ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": null}
        // =================================================================================================================
        this->WrapError(req, error, Errors::GetErrorMessage(error), response);
    }
}

// �����������壺
//      req���ͻ��˳��������˳����͵�req�ǣ�������������
//               �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�����������
//      response���ͻ��˳��������˳����͵��ǣ���ͨ��������
//                    ����˳��򷵻ظ��ͻ��˵�response�ǣ�json��ʽ����Ӧ��������
// �������ܣ�
// ʵ�֣���json-rpc v2.0Э���У��ͻ���������������Ĵ���
// ʲô���������ã�
// ����Ҫͬʱ���Ͷ���������ʱ���ͻ��˿��Է���һ���������������������顣
// ���������õ�����������������ʱ�����������Ҫ����һ��������ͻ��˷��͵���������������Ӧ����Ӧ�������顣
// ��Ӧ���������е�ÿ����Ӧ������������������е�ÿ���������һһ��Ӧ��������֪ͨ���͵��������
// ����˿��Բ����ģ�������˳��������ȵĲ����ԣ���������Щ�������á�
// ���ڣ���Щ���Ӧ����Ӧ���󣬿����������˳�򣬰����ڷ��ص���Ӧ���������У����ԣ��ͻ�����Ҫʹ����Ӧ���������и�����Ӧ�����е�id��Ա����ƥ����֮��Ӧ���������
// ��������ʾ����
// �ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
// --> [
// {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
// {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
// {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
// {"foo": "boo"},
// {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
// {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
// ]
// ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
// <-- [
// {"jsonrpc": "2.0", "result": 7, "id": "1"},
// {"jsonrpc": "2.0", "result": 19, "id": "2"},
// {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"},
// {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
// ]
void RpcProtocolServerV2::HandleBatchRequest(const Json::Value &req, Json::Value &response)
{
    if (req.size() == 0)
    {
        this->WrapError(Json::nullValue, Errors::ERROR_RPC_INVALID_REQUEST,
                        Errors::GetErrorMessage(Errors::ERROR_RPC_INVALID_REQUEST),
                        response);
    }
    else
    {
        for (unsigned int i = 0; i < req.size(); i++)
        {
            Json::Value result;
            // ʵ�֣���json-rpc v2.0Э���У��ͻ�����ͨ��������Ĵ���
            // Json-Rpc V2.0 Э����ͨԶ�̹��̵���ʾ�����£�
            // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
            // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
            // ����˸��ͻ��˷��صģ�json��ʽ����Ӧ�������£�
            // <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
            this->HandleSingleRequest(req[i], result);
            if (result != Json::nullValue)
            {
                response.append(result);
            }
        }
    }
}

// Validate��ȷ�ϣ�֤ʵ
// ����ͨ���У��ڽ��յ���ȷ��Ϣ����ȷ��Ϣ��������ͷ����͵�һ���ź�
// ����˳���ȷ�ϣ���飩���ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request�У������ĸ����ֶ�ֵ���Ƿ�Ϸ�����������Json-Rpc v2.0Э��Ĺ淶��
bool RpcProtocolServerV2::ValidateRequestFields(const Json::Value &request)
{
    if (!request.isObject())
    {
        return false;
    }
    if (!(request.isMember(KEY_REQUEST_METHODNAME) &&
            request[KEY_REQUEST_METHODNAME].isString()))
    {
        return false;
    }

    if (!(request.isMember(KEY_REQUEST_VERSION) &&
            request[KEY_REQUEST_VERSION].isString() &&
            request[KEY_REQUEST_VERSION].asString() == JSON_RPC_VERSION2))
    {
        return false;
    }

    if (request.isMember(KEY_REQUEST_ID) &&
            !(request[KEY_REQUEST_ID].isIntegral() ||
              request[KEY_REQUEST_ID].isString() || request[KEY_REQUEST_ID].isNull()))
    {
        return false;
    }

    if (request.isMember(KEY_REQUEST_PARAMETERS) &&
            !(request[KEY_REQUEST_PARAMETERS].isObject() ||
              request[KEY_REQUEST_PARAMETERS].isArray() ||
              request[KEY_REQUEST_PARAMETERS].isNull()))
    {
        return false;
    }

    return true;
}

// ===================================================================================================================
// �����������壺
// request���ͻ��������˷��͵ģ�json��ʽ���������
// response������˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ����
// result������˳���ִ�У��ͻ��������˷��͵ģ�json��ʽ����������еģ�"method"�ֶδ�ŵĺ����ģ�������
// ===================================================================================================================
// ����������⣺
// Wrap������
// ���������������rpc����:
// �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
// --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": "1"}
// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
// <-- {"jsonrpc": "2.0", "result": 19, "id": "1"}
// ===================================================================================================================
// ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ����ȷ����Ӧ����response���ظ�����
// ʹ��request��result���й��죬����Ľ������ŵ�response��
// ��ȷ����Ӧ����response���ظ�����{"jsonrpc": "2.0", "result": 19, "id": "1"}
// ===================================================================================================================
void RpcProtocolServerV2::WrapResult(const Json::Value &request,
                                     Json::Value &response,
                                     Json::Value &result)
{
    response[KEY_REQUEST_VERSION] = JSON_RPC_VERSION2;
    response[KEY_RESPONSE_RESULT] = result;
    response[KEY_REQUEST_ID] = request[KEY_REQUEST_ID];
}

// Wrap������
// ���÷�����в����ڵķ�����rpc����:
// �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
// --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
// <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// =================================================================================================================
// ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result������Ľ������ŵ�result��
// ʹ��request, code��message���й��죬����Ľ������ŵ�result��
// ��1����һ�ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// ��2���ڶ��ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": null}
// =================================================================================================================
void RpcProtocolServerV2::WrapError(const Json::Value &request, int code,
                                    const string &message,
                                    Json::Value &result)
{
    result["jsonrpc"] = "2.0";
    result["error"]["code"] = code;
    result["error"]["message"] = message;

    if (request.isObject() && request.isMember("id") &&
            (request["id"].isNull() || request["id"].isIntegral() ||
             request["id"].isString()))
    {
        result["id"] = request["id"];
    }
    else
    {
        result["id"] = Json::nullValue;
    }
}

// Wrap������
// ���÷�����в����ڵķ�����rpc����:
// �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
// --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
// <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
// ======================================================================================================================
// ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result������Ľ������ŵ�result��
// ʹ��request��exception���й��죬����Ľ������ŵ�result��
// ��1����һ�ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
// ��2���ڶ��ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
// ======================================================================================================================
void RpcProtocolServerV2::WrapException(const Json::Value &request,
                                        const JsonRpcException &exception,
                                        Json::Value &result)
{
    // Wrap������
    // ���÷�����в����ڵķ�����rpc����:
    // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
    // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
    // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
    // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
    // =================================================================================================================
    // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result������Ľ������ŵ�result��
    // ��1����һ�ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
    // ��2���ڶ��ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": null}
    // =================================================================================================================
    this->WrapError(request, exception.GetCode(), exception.GetMessage(), result);// ��ʱresult��error��Ա�У���û��data�ֶ�
    result["error"]["data"] = exception.GetData();// ��result��error��Ա�У�����data�ֶΣ�����������Ϣ
}

// ��ȡ�������ͣ��ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request��
// ��һ���������󣬻���һ��֪ͨ����
procedure_t RpcProtocolServerV2::GetRequestType(const Json::Value &request)
{
    if (request.isMember(KEY_REQUEST_ID))
    {
        return RPC_METHOD;// ��������
    }
    return RPC_NOTIFICATION;// ֪ͨ����
}
