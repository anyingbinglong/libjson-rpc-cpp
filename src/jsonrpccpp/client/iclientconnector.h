/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    abstractclientconnector.h
 * @date    02.01.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_CLIENTCONNECTOR_H_
#define JSONRPC_CPP_CLIENTCONNECTOR_H_

#include <string>
#include <jsonrpccpp/common/exception.h>

namespace jsonrpc
{
    // 抽象客户端连接器：用于向服务器程序发送RPC消息，并接收服务器程序返回的处理结果
    class IClientConnector
    {
        public:
            virtual ~IClientConnector(){}
            
            // 函数参数含义：
            //      message：客户端程序，向服务端程序，发送的RPC请求消息（对象）
            //      result：在客户端程序，向服务端程序，发起的，rpc调用成功时，使用result，接收，服务端程序，返回给客户端程序的json格式的响应对象
            // 函数功能：
            //      客户端程序向服务端程序，发送message中存放的RPC请求消息（对象），      
            //      并使用result，接收，服务端程序，返回给客户端程序的json格式的响应对象
            virtual void SendRPCMessage(const std::string& message, std::string& result)  = 0;
    };
} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_CLIENTCONNECTOR_H_ */
