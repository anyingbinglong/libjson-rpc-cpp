/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    batchcall.h
 * @date    15.10.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_BATCHCALL_H
#define JSONRPC_CPP_BATCHCALL_H

#include <jsonrpccpp/common/jsonparser.h>

namespace jsonrpc
{
    // 实现：json-rpc v2.0协议中，客户端批量调用请求
    // 什么是批量调用：
    // 当需要同时发送多个请求对象时，客户端可以发送一个包含所有请求对象的数组。
    // 当批量调用的所有请求对象处理完成时，服务端则需要返回一个包含与客户端发送的请求对象数组相对应的响应对象数组。
    // 响应对象数组中的每个响应对象与请求对象数组中的每个请求对象一一对应，除非是通知类型的请求对象。
    // 服务端可以并发的，以任意顺序和任意宽度的并行性，来处理这些批量调用。
    // 由于，这些相对应的响应对象，可以以任意的顺序，包含在返回的响应对象数组中，所以，客户端需要使用响应对象数组中各个响应对象中的id成员，来匹配与之对应的请求对象。
    // 批量调用示例：
    // ============================================================================
    // 这个类主要实现的功能，就是构建：客户端程序向服务端程序发送的，json格式的RPC请求对象数组
    // ============================================================================
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
    class BatchCall
    {
    public:
        BatchCall();

        /**
         * @brief addCall
         * @param methodname
         * @param params
         * @param isNotification
         * @return the id of the geneared request inside the batchcall
         */
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
        int addCall(const std::string &methodname, const Json::Value &params, bool isNotification = false);

        // 函数参数含义：
        //      （1）fast = true：将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成不排版的json格式字符串，用于在网络上传输，减少传输数据量
        //      （2）fast = false：将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成排版的json格式字符串
        // 函数功能：
        //      （1）将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成不排版的json格式字符串，用于在网络上传输，减少传输数据量
        //      （2）将客户端程序，发送给服务端程序的，RPC批量调用请求对象数组this->result，变成排版的json格式字符串
        std::string toString(bool fast = true) const;

    private:
        // 存放：客户端程序，发送给服务端程序的，RPC批量调用请求对象数组
        // 客户端程序向服务端程序发送的，json格式的RPC请求对象数组，示例如下:
        // --> [
        // {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
        // {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
        // {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
        // {"foo": "boo"},
        // {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
        // {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
        // ]
        Json::Value result;
        // 存放：向客户端程序，发送给服务端程序的，RPC批量调用请求对象数组中，增加一个请求对象的，id成员的值
        int id;
    };
}

#endif // JSONRPC_CPP_BATCHCALL_H
