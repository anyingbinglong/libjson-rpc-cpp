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
    // RpcProtocolClient(version)的含义：
    // json-rpc v2.0客户端协议实现：
    // 1).构建RPC请求对象
    // 2).对服务端程序，返回给客户端程序的json格式的响应对象（回复）value，进行分析，
    //      （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
    //      （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(value)，
    //          将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
    // 详细内容，可以参看rpcprotocolclient.h文件内容，进行理解
    this->protocol = new RpcProtocolClient(version);
}

Client::~Client()
{
    delete this->protocol;
}

// 函数参数含义：
//      name：客户端程序请求的方法（函数）的名称
//      parameter：传给客户端程序请求的方法（函数）的参数
//      result：在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
// 函数功能：调用远程方法
//      1).构建RPC请求对象
//      2).客户端程序向服务端程序，发送request中存放的RPC请求消息（对象），
//      并使用response，接收，服务端程序，返回给客户端程序的json格式的响应对象
//      3).对服务端程序，返回给客户端程序的json格式的响应对象（回复）response，进行分析，
//          （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
//          （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(response)，
//              将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
void Client::CallMethod(const std::string &name, const Json::Value &parameter,
                        Json::Value &result)
{
    std::string request, response;
    // protocol->BuildRequest函数功能：构建RPC请求对象
    // 函数参数含义：
    //      name：客户端程序请求的方法（函数）的名称
    //      parameter：传给客户端程序请求的方法（函数）的参数
    //      request：构建好的json格式的RPC请求对象，存放到这里
    //      isNotification：通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
    //                      该参数，用于控制是否构建一个通知
    // request是一个不排版的json格式字符串
    protocol->BuildRequest(name, parameter, request, false);

    // connector抽象客户端程序连接器：用于向服务器程序发送RPC消息，并接收服务器程序返回的处理结果
    // connector.SendRPCMessage函数功能：
    // 客户端程序向服务端程序，发送request中存放的RPC请求消息（对象），
    // 并使用response，接收，服务端程序，返回给客户端程序的json格式的响应对象
    connector.SendRPCMessage(request, response);

    // protocol->HandleResponse函数功能：
    // 对服务端程序，返回给客户端程序的json格式的响应对象（回复）response，进行分析，
    // （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
    // （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(response)，
    //      将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
    protocol->HandleResponse(response, result);
}

void Client::CallProcedures(const BatchCall &calls, BatchResponse &result)
{
    std::string request, response;
    // 将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组calls，变成不排版的json格式字符串，用于在网络上传输，减少传输数据量
    request = calls.toString();
    // connector抽象客户端程序连接器：用于向服务器程序发送RPC消息，并接收服务器程序返回的处理结果
    // connector.SendRPCMessage函数功能：
    // 客户端程序向服务端程序，发送request中存放的RPC请求消息（对象）数组，
    // 并使用response，接收，服务端程序，返回给客户端程序的json格式的响应对象数组
    connector.SendRPCMessage(request, response);
    // 使用Json::Reader将回复字符串response解析为Json格式
    Json::Reader reader;
    // 使用Json::Reader将回复字符串response解析为Json格式后，解析的结果，存放到了这里
    Json::Value tmpresult;

    // 使用Json::Reader将回复字符串response解析为Json格式，并将结果，存放到tmpresult中
    if (!reader.parse(response, tmpresult) || !tmpresult.isArray())
    {
        throw JsonRpcException(Errors::ERROR_CLIENT_INVALID_RESPONSE,
                               "Array expected.");
    }

    // 分析：服务端程序，返回给客户端程序的json格式的响应对象数组tmpresult
    for (unsigned int i = 0; i < tmpresult.size(); i++)
    {
        if (tmpresult[i].isObject())
        {
            Json::Value singleResult;
            try
            {
                // protocol->HandleResponse函数功能：
                // 对服务端程序，返回给客户端程序的json格式的响应对象（回复）tmpresult[i]，进行分析，
                // （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到singleResult中
                // （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(tmpresult[i])，
                //     将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
                Json::Value id = this->protocol->HandleResponse(tmpresult[i], singleResult);
                // 函数参数含义：
                //      id：向服务端程序返回给客户端的，json格式的响应对象数组中，新增加的响应对象的id成员
                //      singleResult：向服务端程序返回给客户端的，json格式的响应对象数组中，新增加的响应对象的具体内容，存放到这里了
                //      isError：标识，向服务端程序返回给客户端的，json格式的响应对象数组中，新增加的响应对象的具体内容中，是否包含"error"成员
                // 函数功能：
                //      向服务端程序返回给客户端的，json格式的响应对象数组中，增加一个响应对象
                // ==================================================================================
                // 向服务端程序返回给客户端的，json格式的响应对象数组中，增加一个响应对象示例：
                // 服务端程序返回给客户端的，json格式的响应对象数组，如下：
                // <-- [
                // {"jsonrpc": "2.0", "result": 7, "id": "1"},
                // {"jsonrpc": "2.0", "result": 19, "id": "2"},
                // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
                // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
                // ]
                // ==================================================================================
                // 向上面的json格式的RPC响应对象数组中，增加如下一个响应对象：
                // {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
                // ==================================================================================
                // 增加完成后，服务端程序返回给客户端的，json格式的响应对象数组，如下：
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

// 函数参数含义：
//      name：客户端程序请求的方法（函数）的名称
//      parameter：传给客户端程序请求的方法（函数）的参数
// 函数返回值的含义：
//      在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到了该返回值中
// 函数功能：调用远程方法
Json::Value Client::CallMethod(const std::string &name,
                               const Json::Value &parameter)
{
    Json::Value result;
    // 函数参数含义：
    //      name：客户端程序请求的方法（函数）的名称
    //      parameter：传给客户端程序请求的方法（函数）的参数
    //      result：在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
    // 函数功能：调用远程方法
    //      1).构建RPC请求对象
    //      2).客户端程序向服务端程序，发送request中存放的RPC请求消息（对象），
    //      并使用response，接收，服务端程序，返回给客户端程序的json格式的响应对象
    //      3).对服务端程序，返回给客户端程序的json格式的响应对象（回复）response，进行分析，
    //          （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
    //          （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(response)，
    //              将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
    this->CallMethod(name, parameter, result);
    return result;
}

// 函数参数含义：
//      name：客户端程序请求的远程通知（函数）的名称
//      parameter：传给客户端程序请求的远程通知（函数）的参数
// 函数功能：调用远程通知
//      1).构建RPC请求对象
//      2).客户端程序向服务端程序，发送request中存放的RPC请求消息（对象），
//         并使用response，接收，服务端程序，返回给客户端程序的json格式的响应对象
void Client::CallNotification(const std::string &name,
                              const Json::Value &parameter)
{
    std::string request, response;
    // protocol->BuildRequest函数功能：构建RPC请求对象
    // 函数参数含义：
    //      name：客户端程序请求的方法（函数）的名称
    //      parameter：传给客户端程序请求的方法（函数）的参数
    //      request：构建好的json格式的RPC请求对象，存放到这里
    //      isNotification：通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
    //                      该参数，用于控制是否构建一个通知
    // request是一个不排版的json格式字符串
    protocol->BuildRequest(name, parameter, request, true);

    // connector抽象客户端程序连接器：用于向服务器程序发送RPC消息，并接收服务器程序返回的处理结果
    // connector.SendRPCMessage函数功能：
    // 客户端程序向服务端程序，发送request中存放的RPC请求消息（对象），
    // 并使用response，接收，服务端程序，返回给客户端程序的json格式的响应对象
    connector.SendRPCMessage(request, response);
}
