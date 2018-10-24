/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    batchresponse.h
 * @date    10/9/2014
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_BATCHRESPONSE_H
#define JSONRPC_BATCHRESPONSE_H

#include <map>
#include <jsonrpccpp/common/jsonparser.h>

namespace jsonrpc
{
    /**
     * @brief The BatchResponse class provides a simple interface for handling batch responses.
     */
    // 实现：json-rpc v2.0协议中，客户端批量响应（回复）
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
    // ============================================================================
    // 这个类主要实现的功能，就是构建：服务端程序返回给客户端的，json格式的响应对象数组
    // ============================================================================
    // 服务端程序返回给客户端的，json格式的响应对象数组，如下：
    // <-- [
    // {"jsonrpc": "2.0", "result": 7, "id": "1"},
    // {"jsonrpc": "2.0", "result": 19, "id": "2"},
    // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
    // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"},
    // {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
    // ]
    class BatchResponse
    {
    public:
        BatchResponse();

        /**
         * @brief addResponse method is used only internally by the framework
         * @param id
         * @param response
         * @param isError
         */
        // 函数参数含义：
        //      id：向服务端程序返回给客户端的，json格式的响应对象数组中，新增加的响应对象的id成员
        //      response：向服务端程序返回给客户端的，json格式的响应对象数组中，新增加的响应对象的具体内容，存放到这里了
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
        void addResponse(Json::Value &id, Json::Value response, bool isError = false);

        /**
         * @brief getResult method gets the result for a given request id (returned by BatchCall::addCall.
         * You should always invoke getErrorCode() first to check if the result is valid.
         * @param id
         * @return
         */
        // 函数参数含义：想要获取的服务端程序返回给客户端的，json格式的响应对象的，id成员的值
        // 返回值含义：想要获取的服务端程序返回给客户端的，json格式的响应对象的值
        // 函数功能：
        // responses的作用：存放，服务端程序返回给客户端的，json格式的响应对象数组
        // 服务端程序返回给客户端的，json格式的响应对象数组，示例如下：
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // responses底层的红黑树的每个节点中，真实存放的内容如下：
        // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
        // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
        // =========================================================================================================
        // 该函数功能：从responses中，获取一个服务端程序返回给客户端的，json格式的响应对象
        // =========================================================================================================
        Json::Value getResult(int id);

        // 函数参数含义：
        //      （1）id：想要获取的服务端程序返回给客户端的，json格式的响应对象的，id成员的值
        //      （2）result：想要获取的服务端程序返回给客户端的，json格式的响应对象的值，存放到了这里
        // 函数功能：
        // responses的作用：存放，服务端程序返回给客户端的，json格式的响应对象数组
        // 服务端程序返回给客户端的，json格式的响应对象数组，示例如下：
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // responses底层的红黑树的每个节点中，真实存放的内容如下：
        // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
        // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
        // =========================================================================================================
        // 该函数功能：从responses中，获取一个服务端程序返回给客户端的，json格式的响应对象
        // =========================================================================================================
        void getResult(Json::Value &id, Json::Value &result);

        /**
         * @brief getErrorCode method checks if for a given id, an error occurred in the batch request.
         * @param id
         */
        // 函数参数含义：服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象的id成员的值
        // 函数返回值含义：服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象中的，"code"成员的值
        // 函数功能：
        // 获取服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象中的，"code"成员的值
        // 出错的rpc批量调用示例:
        // 客户端程序向服务端程序发送的，json格式的RPC请求对象数组，如下:
        // --> [
        // {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
        // {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"}
        // ]
        // 服务端程序返回给客户端的，json格式的响应对象数组，如下：
        // <-- [
        // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": "2"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        int getErrorCode(Json::Value &id);

        /**
         * @brief getErrorMessage method gets the corresponding error message.
         * @param id
         * @return the error message in case of an error, an empty string if no error was found for the provided id.
         */
        // 函数参数含义：服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象的id成员的值
        // 函数返回值含义：服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象中的，"message"成员的值
        // 函数功能：
        // 获取服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象中的，"message"成员的值
        // 出错的rpc批量调用示例:
        // 客户端程序向服务端程序发送的，json格式的RPC请求对象数组，如下:
        // --> [
        // {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
        // {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"}
        // ]
        // 服务端程序返回给客户端的，json格式的响应对象数组，如下：
        // <-- [
        // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": "2"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        std::string getErrorMessage(Json::Value &id);
        std::string getErrorMessage(int id);

        bool hasErrors();

    private:
        // responses的作用：存放，服务端程序返回给客户端的，json格式的响应对象数组
        // 服务端程序返回给客户端的，json格式的响应对象数组，示例如下：
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // responses底层的红黑树的每个节点中，真实存放的内容如下：
        // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
        // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
        std::map<Json::Value, Json::Value> responses;
        
        // errorResponses的作用：存放，服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象的，id成员的值
        // 服务端程序返回给客户端的，json格式的响应对象数组，示例如下：
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // errorResponses底层的数组中，真实存放的内容如下：
        // [{"id": "1"}, {"id": "5"}]
        std::vector<Json::Value> errorResponses;

    };

} // namespace jsonrpc

#endif // JSONRPC_BATCHRESPONSE_H
