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
// 请求对象
const std::string RpcProtocolClient::KEY_PROTOCOL_VERSION = "jsonrpc";
const std::string RpcProtocolClient::KEY_PROCEDURE_NAME = "method";
const std::string RpcProtocolClient::KEY_ID = "id";
const std::string RpcProtocolClient::KEY_PARAMETER = "params";
const std::string RpcProtocolClient::KEY_AUTH = "auth";
// 响应对象
const std::string RpcProtocolClient::KEY_RESULT = "result";
const std::string RpcProtocolClient::KEY_ERROR = "error";
// 错误对象
const std::string RpcProtocolClient::KEY_ERROR_CODE = "code";
const std::string RpcProtocolClient::KEY_ERROR_MESSAGE = "message";
const std::string RpcProtocolClient::KEY_ERROR_DATA = "data";

RpcProtocolClient::RpcProtocolClient(clientVersion_t version)
    : version(version) {}

// 函数功能：构建RPC请求对象
// 函数参数含义：
//      method：客户端程序请求的方法（函数）的名称
//      parameter：传给客户端程序请求的方法（函数）的参数
//      result：构建好的json格式的RPC请求对象，存放到这里
//      isNotification：通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
//                      该参数，用于控制是否构建一个通知
void RpcProtocolClient::BuildRequest(const std::string &method,
                                     const Json::Value &parameter,
                                     std::string &result, bool isNotification)
{
    Json::Value request;// 构建好的json格式的RPC请求对象，存放到这里
    Json::FastWriter writer;
    // 真正实施构建json格式的RPC请求对象
    this->BuildRequest(1, method, parameter, request, isNotification);
    result = writer.write(request);// 将request变成不排版的json格式字符串，用于在网络上传输，减少传输数据量
}

// 处理服务端返回给客户端的json格式的响应对象
// response：服务端返回给客户端的响应对象
// result：存放服务端程序返回的调用函数的执行结果
void RpcProtocolClient::HandleResponse(const std::string &response, Json::Value &result)
{
    Json::Reader reader;// 使用Json::Reader将回复字符串response解析为Json格式
    Json::Value value;// 使用Json::Reader将回复字符串response解析为Json格式后，解析的结果，存放到了这里
    if (reader.parse(response, value))// 使用Json::Reader将回复字符串response解析为Json格式，并将结果，存放到value中
    {
        // ==========================================================================================================
		// 下面这个函数的作用，就是对服务端程序，返回给客户端程序的json格式的响应对象（回复）value，进行分析，
		// （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
		// （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(value)，
		//      将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
		// ==========================================================================================================
        this->HandleResponse(value, result);
    }
    else
    {
        throw JsonRpcException(Errors::ERROR_RPC_JSON_PARSE_ERROR, " " + response);
    }
}

// 服务端返回给客户端的，json格式的响应对象，是按照如下方式，在服务端程序中进行构造的：
// 当发起一个rpc调用时，除通知之外，服务端都必须回复响应。
// 响应表示为一个JSON对象，使用以下成员：
// jsonrpc
//     指定JSON-RPC协议版本的字符串，必须准确写为“2.0”
// result
//     该成员在rpc调用成功时，服务端程序，返回给客户端程序的json格式的响应对象中必须包含该成员。
//     当调用服务端程序方法引起错误时，服务端程序，返回给客户端程序的json格式的响应对象中不可以包含该成员。
//     服务端中的被调用方法决定了该成员的值。
// error
//     该成员在rpc调用失败时，服务端程序，返回给客户端程序的json格式的响应对象中必须包含该成员。
//     该成员在rpc调用成功时，服务端程序，返回给客户端程序的json格式的响应对象中不可以包含该成员。
//     该成员的参数值，必须为5.1中定义的错误对象。
// id
//     该成员必须包含。
//     该成员值必须与请求对象中的id成员值一致。
//     若检查出，请求对象中，包含错误信息（例如参数错误或无效请求），则该值必须为空值。
//     服务端程序，返回给客户端程序的json格式的响应对象中，必须包含result或error成员，但两个成员不能被同时包含。
// 5.1错误对象
//     当一个rpc调用遇到错误时，返回的响应对象必须包含错误成员参数，并且为带有下列成员参数的对象：
// code
//     使用数值表示该异常的错误类型。 必须为整数。
// message
//     对该错误的简单描述字符串。 该描述应尽量限定在简短的一句话。
// data
//     包含关于错误附加信息的基本类型或结构化类型。该成员可忽略。 该成员值由服务端定义（例如详细的错误信息，嵌套的错误等）。
// ==========================================================================================================
// 下面这个函数的作用，就是对服务端程序，返回给客户端程序的json格式的响应对象（回复）value，进行分析，
// （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
// （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(value)，
//      将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
// ==========================================================================================================
Json::Value RpcProtocolClient::HandleResponse(const Json::Value &value, Json::Value &result)
{
    // 判断：服务端程序，返回给客户端程序的json格式的响应对象（回复）value，是一个正确（可用）的json格式的响应对象（回复）
    if (this->ValidateResponse(value))// 服务端程序，返回给客户端程序的json格式的响应对象（回复）value，
    {                                 // 是一个正确（可用）的json格式的响应对象（回复）
        // 服务端返回给客户端的，json格式的响应对象，中有“error”成员，例如:
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // 意味着：客户端程序，向服务端程序，发起的，rpc调用失败
        if (this->HasError(value))      {
            this->throwErrorException(value);
        }
        else// 服务端返回给客户端的，json格式的响应对象，中没有“error”成员，必有“result”成员
        {   // 意味着：客户端程序，向服务端程序，发起的，rpc调用成功
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

// 函数功能：真正实施构建json格式的RPC请求对象
// 函数参数含义：
//      method：客户端程序请求的方法（函数）的名称
//      parameter：传给客户端程序请求的方法（函数）的参数
//      result：构建好的json格式的RPC请求对象，存放到这里
//      isNotification：通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
//                      该参数，用于控制是否构建一个通知
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
    // 通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
    if (!isNotification)// 不是通知，意味着，该json格式的RPC请求对象中包含“id”成员
    {
        result[KEY_ID] = id;
    }
    else if (this->version == JSONRPC_CLIENT_V1)
    {
        result[KEY_ID] = Json::nullValue;
    }
}

// 调用服务端中不存在的方法的rpc调用:
// 客户端向服务端发送的，json格式的RPC请求对象，如下:
// --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
// 服务端返回给客户端的，json格式的响应对象，如下:
// <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
// =================================================================================================================
// 下面这个函数的作用，就是对服务端程序，返回给客户端程序的json格式的响应对象（回复）response，进行分析，
// （1）将response中的，"error": {"code": -32601, "message": "Method not found"，"data": }，内容，作为异常信息，抛出
// （2）将response中的，"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
// （3）将response中的，"error": {"code": -32601}，内容，作为异常信息，抛出
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

// 判断：服务端程序，返回给客户端程序的json格式的响应对象（回复），是一个正确（可用）的json格式的响应对象（回复）
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

// 判断：
// 服务端返回给客户端的，json格式的响应对象中，是否有“error”成员，例如:
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
