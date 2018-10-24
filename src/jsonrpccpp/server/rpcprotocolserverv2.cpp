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

// 函数参数含义：
//      req：（1）客户端程序向服务端程序发送的req是：批量调用请求
//               客户端程序向服务端程序发送的req是：json格式的RPC请求对象数组
//           （2）客户端程序向服务端程序发送的req是：普通调用请求
//               客户端程序向服务端程序发送的req是：json格式的RPC请求对象
//      response：（1）客户端程序向服务端程序发送的是：普通调用请求
//                    服务端程序返回给客户端的response是：json格式的响应对象数组
//                （2）客户端程序向服务端程序发送的是：普通调用请求
//                    服务端程序返回给客户端的response是：json格式的响应对象
// 函数功能：
// 实现：对json-rpc v2.0协议中，客户端普通调用请求的处理
//      对客户端程序向服务端程序发送的req -- json格式的RPC请求对象进行分析与处理，
//      并将处理的结果 -- json格式的响应对象，放到response中
// 实现：对json-rpc v2.0协议中，客户端批量调用请求的处理
//      对客户端程序向服务端程序发送的req -- json格式的RPC请求对象数组进行分析与处理，
//      并将处理的结果 -- json格式的响应对象数组，放到response中
void RpcProtocolServerV2::HandleJsonRequest(const Json::Value &req, Json::Value &response)
{
    // It could be a Batch Request
    if (req.isArray())// 客户端程序向服务端程序发送的req是：批量调用请求
    {
        // 客户端程序向服务端程序发送的req是：json格式的RPC请求对象数组
        // 实现：对json-rpc v2.0协议中，客户端批量调用请求的处理
        // 什么是批量调用：
        // 当需要同时发送多个请求对象时，客户端可以发送一个包含所有请求对象的数组。
        // 当批量调用的所有请求对象处理完成时，服务端则需要返回一个包含与客户端发送的请求对象数组相对应的响应对象数组。
        // 响应对象数组中的每个响应对象与请求对象数组中的每个请求对象一一对应，除非是通知类型的请求对象。
        // 服务端可以并发的，以任意顺序和任意宽度的并行性，来处理这些批量调用。
        // 由于，这些相对应的响应对象，可以以任意的顺序，包含在返回的响应对象数组中，所以，客户端需要使用响应对象数组中各个响应对象中的id成员，来匹配与之对应的请求对象。
        // 批量调用示例：
        // 客户端程序向服务端程序发送的，json格式的RPC请求对象数组，如下:
        // --> [
        // {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
        // {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
        // {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
        // {"foo": "boo"},
        // {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
        // {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
        // ]
        // 服务端程序返回给客户端的，json格式的响应对象数组，如下：
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "result": 19, "id": "2"},
        // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"},
        // {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
        // ]
        this->HandleBatchRequest(req, response);
    } // It could be a simple Request
    else if (req.isObject())// 客户端程序向服务端程序发送的req是：普通调用请求
    {
        // 客户端程序向服务端程序发送的req是：json格式的RPC请求对象
        // 实现：对json-rpc v2.0协议中，客户端普通调用请求的处理
        // Json-Rpc V2.0 协议普通远程过程调用示例如下：
        // 客户端向服务端发送的，json格式的RPC请求对象，如下:
        // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
        // 服务端给客户端返回的，json格式的响应对象，如下：
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

// 函数参数含义：
//      req：客户端程序向服务端程序发送的req是：普通调用请求
//           客户端程序向服务端程序发送的req是：json格式的RPC请求对象
//      response：客户端程序向服务端程序发送的是：普通调用请求
//                服务端程序返回给客户端的response是：json格式的响应对象
// 函数功能：
// 实现：对json-rpc v2.0协议中，客户端普通调用请求的处理
// Json-Rpc V2.0 协议普通远程过程调用示例如下：
// 客户端向服务端发送的，json格式的RPC请求对象，如下:
// --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
// 服务端给客户端返回的，json格式的响应对象，如下：
// <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
void RpcProtocolServerV2::HandleSingleRequest(const Json::Value &req, Json::Value &response)
{
    // 函数参数含义：
    //      request：客户端程序向服务端程序发送的json格式的RPC请求对象（数组）
    // ===============================================================================================
    // 函数功能
    // Validate：确认，证实
    // 网络通信中，在接收到正确信息或正确信息分组后，向发送方回送的一个信号
    // 服务端程序，确认（检查），客户端程序，发送给服务端程序的Json格式的请求对象request中，包含的各个字段值，
    // 是否合法（即：符合Json-Rpc v2.0协议的规范）
    // ===============================================================================================
    // 下面这个函数，是 class RpcProtocolServerV2 类的，父类class AbstractProtocolHandler中的成员函数
    // ===============================================================================================
    int error = this->ValidateRequest(req);
    if (error == 0)
    {
        try
        {
            // 函数参数含义：
            //      req：客户端程序向服务端程序发送的json格式的RPC请求对象
            //      response：对客户端程序向服务端程序发送的request
            //                -- json格式的RPC请求对象进行分析与处理的结果 -- json格式的响应对象，放到response中
            // =============================================================================================================
            // 函数功能
            // 实现：对json-rpc v2.0协议中，客户端RPC远程过程调用请求的处理
            // 对客户端程序向服务端程序发送的request -- json格式的RPC请求对象进行分析与处理，
            // 并将处理的结果 -- json格式的响应对象，放到response中
            // =============================================================================================================
            // 下面这个函数，是 class RpcProtocolServerV2 类的，父类class AbstractProtocolHandler中的成员函数
            // =============================================================================================================
            this->ProcessRequest(req, response);
        }
        catch (const JsonRpcException &exc)
        {
            // Wrap：返回
            // 调用服务端中不存在的方法的rpc调用:
            // 客户端向服务端发送的，json格式的RPC请求对象，如下:
            // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
            // 服务端返回给客户端的，json格式的响应对象，如下:
            // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
            // ========================================================================================================================
            // 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，构造的结果，存放到result中
            // 使用request和exception进行构造，构造的结果，存放到result中
            // （1）第一种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
            // （2）第二种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
            // ========================================================================================================================
            this->WrapException(req, exc, response);
        }
    }
    else
    {
        // Wrap：返回
        // 调用服务端中不存在的方法的rpc调用:
        // 客户端向服务端发送的，json格式的RPC请求对象，如下:
        // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
        // 服务端返回给客户端的，json格式的响应对象，如下:
        // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // =================================================================================================================
        // 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，构造的结果，存放到result中
        // 使用request, code和message进行构造，构造的结果，存放到result中
        // （1）第一种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // （2）第二种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": null}
        // =================================================================================================================
        this->WrapError(req, error, Errors::GetErrorMessage(error), response);
    }
}

// 函数参数含义：
//      req：客户端程序向服务端程序发送的req是：批量调用请求
//               客户端程序向服务端程序发送的req是：json格式的RPC请求对象数组
//      response：客户端程序向服务端程序发送的是：普通调用请求
//                    服务端程序返回给客户端的response是：json格式的响应对象数组
// 函数功能：
// 实现：对json-rpc v2.0协议中，客户端批量调用请求的处理
// 什么是批量调用：
// 当需要同时发送多个请求对象时，客户端可以发送一个包含所有请求对象的数组。
// 当批量调用的所有请求对象处理完成时，服务端则需要返回一个包含与客户端发送的请求对象数组相对应的响应对象数组。
// 响应对象数组中的每个响应对象与请求对象数组中的每个请求对象一一对应，除非是通知类型的请求对象。
// 服务端可以并发的，以任意顺序和任意宽度的并行性，来处理这些批量调用。
// 由于，这些相对应的响应对象，可以以任意的顺序，包含在返回的响应对象数组中，所以，客户端需要使用响应对象数组中各个响应对象中的id成员，来匹配与之对应的请求对象。
// 批量调用示例：
// 客户端程序向服务端程序发送的，json格式的RPC请求对象数组，如下:
// --> [
// {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
// {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
// {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
// {"foo": "boo"},
// {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
// {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
// ]
// 服务端程序返回给客户端的，json格式的响应对象数组，如下：
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
            // 实现：对json-rpc v2.0协议中，客户端普通调用请求的处理
            // Json-Rpc V2.0 协议普通远程过程调用示例如下：
            // 客户端向服务端发送的，json格式的RPC请求对象，如下:
            // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
            // 服务端给客户端返回的，json格式的响应对象，如下：
            // <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
            this->HandleSingleRequest(req[i], result);
            if (result != Json::nullValue)
            {
                response.append(result);
            }
        }
    }
}

// Validate：确认，证实
// 网络通信中，在接收到正确信息或正确信息分组后，向发送方回送的一个信号
// 服务端程序，确认（检查），客户端程序，发送给服务端程序的Json格式的请求对象request中，包含的各个字段值，是否合法（即：符合Json-Rpc v2.0协议的规范）
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
// 函数参数含义：
// request：客户端向服务端发送的，json格式的请求对象
// response：服务端返回给客户端的，json格式的响应对象
// result：服务端程序，执行，客户端向服务端发送的，json格式的请求对象中的，"method"字段存放的函数的，计算结果
// ===================================================================================================================
// 函数功能详解：
// Wrap：返回
// 带索引数组参数的rpc调用:
// 客户端向服务端发送的，json格式的RPC请求对象，如下:
// --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": "1"}
// 服务端返回给客户端的，json格式的响应对象，如下:
// <-- {"jsonrpc": "2.0", "result": 19, "id": "1"}
// ===================================================================================================================
// 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的正确的响应对象response（回复），
// 使用request和result进行构造，构造的结果，存放到response中
// 正确的响应对象response（回复）：{"jsonrpc": "2.0", "result": 19, "id": "1"}
// ===================================================================================================================
void RpcProtocolServerV2::WrapResult(const Json::Value &request,
                                     Json::Value &response,
                                     Json::Value &result)
{
    response[KEY_REQUEST_VERSION] = JSON_RPC_VERSION2;
    response[KEY_RESPONSE_RESULT] = result;
    response[KEY_REQUEST_ID] = request[KEY_REQUEST_ID];
}

// Wrap：返回
// 调用服务端中不存在的方法的rpc调用:
// 客户端向服务端发送的，json格式的RPC请求对象，如下:
// --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
// 服务端返回给客户端的，json格式的响应对象，如下:
// <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// =================================================================================================================
// 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，构造的结果，存放到result中
// 使用request, code和message进行构造，构造的结果，存放到result中
// （1）第一种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// （2）第二种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": null}
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

// Wrap：返回
// 调用服务端中不存在的方法的rpc调用:
// 客户端向服务端发送的，json格式的RPC请求对象，如下:
// --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
// 服务端返回给客户端的，json格式的响应对象，如下:
// <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
// ======================================================================================================================
// 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，构造的结果，存放到result中
// 使用request和exception进行构造，构造的结果，存放到result中
// （1）第一种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
// （2）第二种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
// ======================================================================================================================
void RpcProtocolServerV2::WrapException(const Json::Value &request,
                                        const JsonRpcException &exception,
                                        Json::Value &result)
{
    // Wrap：返回
    // 调用服务端中不存在的方法的rpc调用:
    // 客户端向服务端发送的，json格式的RPC请求对象，如下:
    // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
    // 服务端返回给客户端的，json格式的响应对象，如下:
    // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
    // =================================================================================================================
    // 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，构造的结果，存放到result中
    // （1）第一种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
    // （2）第二种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": null}
    // =================================================================================================================
    this->WrapError(request, exception.GetCode(), exception.GetMessage(), result);// 此时result的error成员中，还没有data字段
    result["error"]["data"] = exception.GetData();// 在result的error成员中，增加data字段，并添加相关信息
}

// 获取请求类型：客户端程序，发送给服务端程序的Json格式的请求对象request，
// 是一个方法请求，还是一个通知请求
procedure_t RpcProtocolServerV2::GetRequestType(const Json::Value &request)
{
    if (request.isMember(KEY_REQUEST_ID))
    {
        return RPC_METHOD;// 方法请求
    }
    return RPC_NOTIFICATION;// 通知请求
}
