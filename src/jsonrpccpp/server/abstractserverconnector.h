/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    abstractserverconnector.h
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_SERVERCONNECTOR_H_
#define JSONRPC_CPP_SERVERCONNECTOR_H_

#include "iclientconnectionhandler.h"
#include <string>

namespace jsonrpc
{
    // 抽象服务端连接器
    // 实现服务端程序监听客户程序发来的RPC消息，
    // 并向客户程序发送自己对RPC请求的处理结果
    class AbstractServerConnector
    {
    public:
        AbstractServerConnector();
        virtual ~AbstractServerConnector();

        /**
         * This method should signal the Connector to start waiting for requests, in
         * any way that is appropriate for the derived connector class.
         * If something went wrong, this method should return false, otherwise true.
         */
        virtual bool StartListening() = 0;
        /**
         * This method should signal the Connector to stop waiting for requests, in
         * any way that is appropriate for the derived connector class.
         * If something went wrong, this method should return false, otherwise true.
         */
        virtual bool StopListening() = 0;
        
        // 函数参数含义：
        //      request：客户端程序向服务端程序，发送的RPC请求消息（对象），存放在request中
        //      response：服务端程序，返回给客户端程序的json格式的响应对象，存放到response中
        // 函数功能：
        //      客户端程序向服务端程序，发送request中存放的RPC请求消息（对象），
        //      并使用response，接收，服务端程序，返回给客户端程序的json格式的响应对象
        // Json-Rpc V2.0 协议远程过程调用示例如下：
        // 客户端向服务端发送的，json格式的RPC请求对象，如下:
        // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
        // 服务端给客户端返回的，json格式的响应对象，如下：
        // <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
        void ProcessRequest(const std::string &request, std::string &response);
        
        // 设置：抽象客户端连接处理器
        // 抽象客户端连接处理器：
        // 主要用来实现服务端程序处理客户端程序发来的
        // RPC请求，即：调用请求对象中描述的方法（函数），
        // 执行该函数，计算出一个结果
        void SetHandler(IClientConnectionHandler *handler);
        // 获取：抽象客户端连接处理器
        IClientConnectionHandler *GetHandler();

    private:
        // =========================================================================================
        // IClientConnectionHandler 类的功能
        // 抽象客户端连接处理器：
        // 主要用来实现服务端程序处理客户端程序发来的
        // RPC请求，即：调用请求对象中描述的方法（函数），
        // 执行该函数，计算出一个结果
        // =========================================================================================
        // IClientConnectionHandler 类的作用
        // 这个类，是：桥模式右侧，最顶层，最抽象的类，
        // （1）这个类，统一将桥模式右侧所有的内容，都描述了出来，
        // 所以，桥模式左侧，只需包含这个类的一个指针，就可以实现通过这一个指针，
        // 使用桥模式右侧所有的内容的效果。
        // （2）同时，在这个类之下的所有的类，如果在未来发生变化，都不会对桥模式左侧，造成任何影响，
        // 这就实现了桥模式左侧和右侧的解耦
        // =========================================================================================
        IClientConnectionHandler *handler;
    };

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_ERVERCONNECTOR_H_ */
