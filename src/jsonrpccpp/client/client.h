/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    client.h
 * @date    03.01.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_CLIENT_H_
#define JSONRPC_CPP_CLIENT_H_

#include "iclientconnector.h"
#include "batchcall.h"
#include "batchresponse.h"
#include <jsonrpccpp/common/jsonparser.h>

#include <vector>
#include <map>

namespace jsonrpc
{
    class RpcProtocolClient;
    // json-rpc 协议的客户端程序的版本 v1.0 或 v2.0
    typedef enum {JSONRPC_CLIENT_V1, JSONRPC_CLIENT_V2} clientVersion_t;
    
    // json-rpc v1.0和 v2.0客户端程序实现
    class Client
    {
    public:
        Client(IClientConnector &connector, clientVersion_t version = JSONRPC_CLIENT_V2);
        virtual ~Client();
        
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
        void CallMethod(const std::string &name, const Json::Value &parameter, Json::Value &result);
        // 函数参数含义：
        //      name：客户端程序请求的方法（函数）的名称
        //      parameter：传给客户端程序请求的方法（函数）的参数
        // 函数返回值的含义：
        //      在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到了该返回值中
        // 函数功能：调用远程方法
        Json::Value CallMethod(const std::string &name, const Json::Value &parameter);

        // 远程过程的含义：
        // （1）远程方法
        // （2）远程通知
        // 调用远程过程
        void CallProcedures(const BatchCall &calls, BatchResponse &response);
        BatchResponse CallProcedures(const BatchCall &calls) ;

        // 函数参数含义：
        //      name：客户端程序请求的远程通知（函数）的名称
        //      parameter：传给客户端程序请求的远程通知（函数）的参数
        // 函数功能：调用远程通知
        //      1).构建RPC请求对象
        //      2).客户端程序向服务端程序，发送request中存放的RPC请求消息（对象），     
        //         并使用response，接收，服务端程序，返回给客户端程序的json格式的响应对象
        void CallNotification(const std::string &name, const Json::Value &parameter);

    private:
        // 抽象客户端程序连接器：用于向服务器程序发送RPC消息，并接收服务器程序返回的处理结果
        IClientConnector  &connector;
        
        // RpcProtocolClient的含义：
        // json-rpc v2.0客户端协议实现：
        // 1).构建RPC请求对象
        // 2).对服务端程序，返回给客户端程序的json格式的响应对象（回复）value，进行分析，
        //      （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
        //      （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(value)，
        //          将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
        // 详细内容，可以参看rpcprotocolclient.h文件内容，进行理解
        RpcProtocolClient *protocol;
    };

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_CLIENT_H_ */
