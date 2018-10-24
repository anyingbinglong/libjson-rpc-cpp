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

// 函数参数含义：
//      methodname：客户端程序请求的方法（函数）的名称
//      params：传给客户端程序请求的方法（函数）的参数
//      isNotification：通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
//                      该参数，用于控制是否构建一个通知
// 函数功能：
//      向客户端程序，发送给服务端程序的，RPC批量调用请求对象数组中，增加一个请求对象
// ==================================================================================
// 向客户端程序，发送给服务端程序的，RPC批量调用请求对象数组中，增加一个请求对象示例：
// 客户端程序向服务端程序发送的，json格式的RPC请求对象数组，如下:
// --> [
// {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
// {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
// {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
// {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
// ]
// ==================================================================================
// 向上面的json格式的RPC请求对象数组中，增加如下一个请求对象：
// {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]}
// ==================================================================================
// 增加完成后，客户端程序向服务端程序发送的，json格式的RPC请求对象数组，如下:
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
    Json::Value call;// 存放：新增加的一个请求对象
    // 构建：新增加的一个请求对象
    call[RpcProtocolClient::KEY_PROTOCOL_VERSION] = "2.0";
    call[RpcProtocolClient::KEY_PROCEDURE_NAME] = methodname;
    call[RpcProtocolClient::KEY_PARAMETER] = params;
    // 新增加的请求对象，是通知
    if (!isNotification)
    {
        call[RpcProtocolClient::KEY_ID] = this->id++;
    }
    // 向客户端程序，发送给服务端程序的，RPC批量调用请求对象数组中，增加一个请求对象
    this->result.append(call);
    
    // 新增加的请求对象，是通知
    if (isNotification)
    {
        return -1;
    }
    return call[RpcProtocolClient::KEY_ID].asInt();
}

// 函数参数含义：
//      （1）fast = true：将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成不排版的json格式字符串，用于在网络上传输，减少传输数据量
//      （2）fast = false：将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成排版的json格式字符串
// 函数功能：
//      （1）将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成不排版的json格式字符串，用于在网络上传输，减少传输数据量
//      （2）将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成排版的json格式字符串
string BatchCall::toString(bool fast) const
{
    string result;// 转换好的json格式的RPC请求对象数组，存放到这里
    if (fast)//（1）fast = true：
    {
        Json::FastWriter writer;
        // 将this->result变成不排版的json格式字符串，用于在网络上传输，减少传输数据量
        result = writer.write(this->result);
    }
    else//（2）fast = false
    {
        Json::StyledWriter writer;
        // 将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成排版的json格式字符串
        result = writer.write(this->result);
    }
    return result;
}
