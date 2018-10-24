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
                // Wrap：返回
                // 调用服务端中不存在的方法的rpc调用:
                // 客户端向服务端发送的，json格式的RPC请求对象，如下:
                // --> {"method": "foobar", "id": "1"}
                // 服务端返回给客户端的，json格式的响应对象，如下:
                // <-- {"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
                // ======================================================================================================================
                // 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象response，
                // 使用req和exc进行构造，构造的结果，存放到response中
                // （1）第一种错误对象：{"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
                // （2）第二种错误对象：{"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
                // ======================================================================================================================
                this->WrapException(req, exc, response);
            }
        }
        else
        {
            // Wrap：返回
            // 调用服务端中不存在的方法的rpc调用:
            // 客户端向服务端发送的，json格式的RPC请求对象，如下:
            // --> {"method": "foobar", "id": "1"}
            // 服务端返回给客户端的，json格式的响应对象，如下:
            // <-- {"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
            // =================================================================================================================
            // 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象response，
            // 使用req, error和Errors::GetErrorMessage(error)进行构造，构造的结果，存放到response中
            // （1）第一种错误对象：{"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
            // （2）第二种错误对象：{"error": {"code": -32601, "message": "Method not found"}, "id": null}
            // =================================================================================================================
            this->WrapError(req, error, Errors::GetErrorMessage(error), response);
        }
    }
    else
    {
        // Wrap：返回
        // 调用服务端中不存在的方法的rpc调用:
        // 客户端向服务端发送的，json格式的RPC请求对象，如下:
        // --> {"method": "foobar", "id": "1"}
        // 服务端返回给客户端的，json格式的响应对象，如下:
        // <-- {"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // ====================================================================================================================================================
        // 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象response，
        // 使用Json::nullValue, Errors::ERROR_RPC_INVALID_REQUEST和Errors::GetErrorMessage(Errors::ERROR_RPC_INVALID_REQUEST)进行构造，构造的结果，存放到response中
        // （1）第一种错误对象：{"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // （2）第二种错误对象：{"error": {"code": -32601, "message": "Method not found"}, "id": null}
        // ====================================================================================================================================================
        this->WrapError(Json::nullValue, Errors::ERROR_RPC_INVALID_REQUEST,
                        Errors::GetErrorMessage(Errors::ERROR_RPC_INVALID_REQUEST),
                        response);
    }
}

// Validate：确认，证实
// 网络通信中，在接收到正确信息或正确信息分组后，向发送方回送的一个信号
// 服务端程序，确认（检查），客户端程序，发送给服务端程序的Json格式的请求对象request中，包含的各个字段值，是否合法（即：符合Json-Rpc v1.0协议的规范）
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
// 函数参数含义：
// request：客户端向服务端发送的，json格式的请求对象
// response：服务端返回给客户端的，json格式的响应对象
// retValue：服务端程序，执行，客户端向服务端发送的，json格式的请求对象中的，"method"字段存放的函数的，计算结果
// ================================================================================================================
// 函数功能详解：
// Wrap：返回
// 带索引数组参数的rpc调用:
// 客户端向服务端发送的，json格式的RPC请求对象，如下:
// --> {"method": "subtract", "params": [42, 23], "id": "1"}
// 服务端返回给客户端的，json格式的响应对象，如下:
// <-- {"result": 19, "id": "1"}
// =================================================================================================================
// 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的正确的响应对象response（回复），
// 使用request和retValue进行构造，构造的结果，存放到response中
// 正确的响应对象response（回复）：{"result": 19, "id": "1"}
// =================================================================================================================
void RpcProtocolServerV1::WrapResult(const Json::Value &request,
                                     Json::Value &response,
                                     Json::Value &retValue)
{
    response[KEY_RESPONSE_RESULT] = retValue;
    response[KEY_RESPONSE_ERROR] = Json::nullValue;
    response[KEY_REQUEST_ID] = request[KEY_REQUEST_ID];
}

// Wrap：返回
// 调用服务端中不存在的方法的rpc调用:
// 客户端向服务端发送的，json格式的RPC请求对象，如下:
// --> {"method": "foobar", "id": "1"}
// 服务端返回给客户端的，json格式的响应对象，如下:
// <-- {"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// =================================================================================================================
// 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，
// 使用request, code和message进行构造，构造的结果，存放到result中
// （1）第一种错误对象：{"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// （2）第二种错误对象：{"error": {"code": -32601, "message": "Method not found"}, "id": null}
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

// Wrap：返回
// 调用服务端中不存在的方法的rpc调用:
// 客户端向服务端发送的，json格式的RPC请求对象，如下:
// --> {"method": "foobar", "id": "1"}
// 服务端返回给客户端的，json格式的响应对象，如下:
// <-- {"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
// ======================================================================================================================
// 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，
// 使用request和exception进行构造，构造的结果，存放到result中
// （1）第一种错误对象：{"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
// （2）第二种错误对象：{"error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
// ======================================================================================================================
void RpcProtocolServerV1::WrapException(const Json::Value &request,
                                        const JsonRpcException &exception,
                                        Json::Value &result)
{
    // Wrap：返回
    // 调用服务端中不存在的方法的rpc调用:
    // 客户端向服务端发送的，json格式的RPC请求对象，如下:
    // --> {"method": "foobar", "id": "1"}
    // 服务端返回给客户端的，json格式的响应对象，如下:
    // <-- {"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
    // =================================================================================================================
    // 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，
    // （1）第一种错误对象：{"error": {"code": -32601, "message": "Method not found"}, "id": "1"}
    // （2）第二种错误对象：{"error": {"code": -32601, "message": "Method not found"}, "id": null}
    // =================================================================================================================
    this->WrapError(request, exception.GetCode(), exception.GetMessage(), result);// 此时result的error成员中，还没有data字段
    result["error"]["data"] = exception.GetData();// 在result的error成员中，增加data字段，并添加相关信息
}

// 获取请求类型：客户端程序，发送给服务端程序的Json格式的请求对象request，
// 是一个远程方法请求，还是一个远程通知请求
procedure_t RpcProtocolServerV1::GetRequestType(const Json::Value &request)
{
    if (request[KEY_REQUEST_ID] == Json::nullValue)
    {
        return RPC_NOTIFICATION;// 远程通知请求
    }
    return RPC_METHOD;// 远程方法请求
}
