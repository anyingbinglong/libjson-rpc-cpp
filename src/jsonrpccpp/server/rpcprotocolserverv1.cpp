/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    rpcprotocolserverv1.cpp
* @date    10/23/2014
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#include "rpcprotocolserverv1.h"
#include <jsonrpccpp/common/errors.h>
#include <jsonrpccpp/common/exception.h>

using namespace jsonrpc;

RpcProtocolServerV1::RpcProtocolServerV1(IProcedureInvokationHandler &handler)
    : AbstractProtocolHandler(handler) {}

void RpcProtocolServerV1::HandleJsonRequest(const Json::Value &req, Json::Value &response)
{
    if (req.isObject())
    {
        int error = this->ValidateRequest(req);
        if (error == 0)
        {
            try
            {
                this->ProcessRequest(req, response);
            }
            catch (const JsonRpcException &exc)
            {
                // Wrap������
                // ���÷�����в����ڵķ�����rpc����:
                // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
                // --> {"method": "foobar", "id": "1"}
                // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
                // <-- {"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
                // ======================================================================================================================
                // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������response��
                // ʹ��req��exc���й��죬����Ľ������ŵ�response��
                // ��1����һ�ִ������{"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
                // ��2���ڶ��ִ������{"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
                // ======================================================================================================================
                this->WrapException(req, exc, response);
            }
        }
        else
        {
            // Wrap������
            // ���÷�����в����ڵķ�����rpc����:
            // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
            // --> {"method": "foobar", "id": "1"}
            // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
            // <-- {"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
            // =================================================================================================================
            // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������response��
            // ʹ��req, error��Errors::GetErrorMessage(error)���й��죬����Ľ������ŵ�response��
            // ��1����һ�ִ������{"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
            // ��2���ڶ��ִ������{"error": {"code": -32601, "message": "Method not found"}, "id": null}
            // =================================================================================================================
            this->WrapError(req, error, Errors::GetErrorMessage(error), response);
        }
    }
    else
    {
        // Wrap������
        // ���÷�����в����ڵķ�����rpc����:
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"method": "foobar", "id": "1"}
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
        // <-- {"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // ====================================================================================================================================================
        // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������response��
        // ʹ��Json::nullValue, Errors::ERROR_RPC_INVALID_REQUEST��Errors::GetErrorMessage(Errors::ERROR_RPC_INVALID_REQUEST)���й��죬����Ľ������ŵ�response��
        // ��1����һ�ִ������{"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // ��2���ڶ��ִ������{"error": {"code": -32601, "message": "Method not found"}, "id": null}
        // ====================================================================================================================================================
        this->WrapError(Json::nullValue, Errors::ERROR_RPC_INVALID_REQUEST,
                        Errors::GetErrorMessage(Errors::ERROR_RPC_INVALID_REQUEST),
                        response);
    }
}

// Validate��ȷ�ϣ�֤ʵ
// ����ͨ���У��ڽ��յ���ȷ��Ϣ����ȷ��Ϣ��������ͷ����͵�һ���ź�
// ����˳���ȷ�ϣ���飩���ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request�У������ĸ����ֶ�ֵ���Ƿ�Ϸ�����������Json-Rpc v1.0Э��Ĺ淶��
bool RpcProtocolServerV1::ValidateRequestFields(const Json::Value &request)
{
    if (!(request.isMember(KEY_REQUEST_METHODNAME) &&
            request[KEY_REQUEST_METHODNAME].isString()))
    {
        return false;
    }

    if (!request.isMember(KEY_REQUEST_ID))
    {
        return false;
    }
    if (!request.isMember(KEY_REQUEST_PARAMETERS))
    {
        return false;
    }
    if (!(request[KEY_REQUEST_PARAMETERS].isArray() ||
            request[KEY_REQUEST_PARAMETERS].isNull()))
    {
        return false;
    }

    return true;
}

// ================================================================================================================
// �����������壺
// request���ͻ��������˷��͵ģ�json��ʽ���������
// response������˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ����
// retValue������˳���ִ�У��ͻ��������˷��͵ģ�json��ʽ����������еģ�"method"�ֶδ�ŵĺ����ģ�������
// ================================================================================================================
// ����������⣺
// Wrap������
// ���������������rpc����:
// �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
// --> {"method": "subtract", "params": [42, 23], "id": "1"}
// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
// <-- {"result": 19, "id": "1"}
// =================================================================================================================
// ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ����ȷ����Ӧ����response���ظ�����
// ʹ��request��retValue���й��죬����Ľ������ŵ�response��
// ��ȷ����Ӧ����response���ظ�����{"result": 19, "id": "1"}
// =================================================================================================================
void RpcProtocolServerV1::WrapResult(const Json::Value &request,
                                     Json::Value &response,
                                     Json::Value &retValue)
{
    response[KEY_RESPONSE_RESULT] = retValue;
    response[KEY_RESPONSE_ERROR] = Json::nullValue;
    response[KEY_REQUEST_ID] = request[KEY_REQUEST_ID];
}

// Wrap������
// ���÷�����в����ڵķ�����rpc����:
// �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
// --> {"method": "foobar", "id": "1"}
// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
// <-- {"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// =================================================================================================================
// ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result��
// ʹ��request, code��message���й��죬����Ľ������ŵ�result��
// ��1����һ�ִ������{"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// ��2���ڶ��ִ������{"error": {"code": -32601, "message": "Method not found"}, "id": null}
// =================================================================================================================
void RpcProtocolServerV1::WrapError(const Json::Value &request, int code,
                                    const std::string &message,
                                    Json::Value &result)
{
    result["error"]["code"] = code;
    result["error"]["message"] = message;
    result["result"] = Json::nullValue;
    if (request.isObject() && request.isMember("id"))
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
// --> {"method": "foobar", "id": "1"}
// ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
// <-- {"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
// ======================================================================================================================
// ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result��
// ʹ��request��exception���й��죬����Ľ������ŵ�result��
// ��1����һ�ִ������{"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
// ��2���ڶ��ִ������{"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
// ======================================================================================================================
void RpcProtocolServerV1::WrapException(const Json::Value &request,
                                        const JsonRpcException &exception,
                                        Json::Value &result)
{
    // Wrap������
    // ���÷�����в����ڵķ�����rpc����:
    // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
    // --> {"method": "foobar", "id": "1"}
    // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
    // <-- {"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
    // =================================================================================================================
    // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result��
    // ��1����һ�ִ������{"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
    // ��2���ڶ��ִ������{"error": {"code": -32601, "message": "Method not found"}, "id": null}
    // =================================================================================================================
    this->WrapError(request, exception.GetCode(), exception.GetMessage(), result);// ��ʱresult��error��Ա�У���û��data�ֶ�
    result["error"]["data"] = exception.GetData();// ��result��error��Ա�У�����data�ֶΣ�����������Ϣ
}

// ��ȡ�������ͣ��ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request��
// ��һ��Զ�̷������󣬻���һ��Զ��֪ͨ����
procedure_t RpcProtocolServerV1::GetRequestType(const Json::Value &request)
{
    if (request[KEY_REQUEST_ID] == Json::nullValue)
    {
        return RPC_NOTIFICATION;// Զ��֪ͨ����
    }
    return RPC_METHOD;// Զ�̷�������
}
