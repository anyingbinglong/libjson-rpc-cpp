/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    batchresponse.cpp
* @date    10/9/2014
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#include "batchresponse.h"
#include <algorithm>

using namespace jsonrpc;
using namespace std;

BatchResponse::BatchResponse() {}

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
void BatchResponse::addResponse(Json::Value &id, Json::Value response,
                                bool isError)
{
    if (isError)
    {
        // errorResponses的作用：存放，服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象的，id成员的值
        // 服务端程序返回给客户端的，json格式的响应对象数组，示例如下：
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // errorResponses底层的数组中，真实存放的内容如下：
        // [{"id": "1"}, {"id": "5"}]
        errorResponses.push_back(id);
    }
    // responses的作用：存放，服务端程序返回给客户端的，json格式的响应对象数组
    // 服务端程序返回给客户端的，json格式的响应对象数组，示例如下：
    // <-- [
    // {"jsonrpc": "2.0", "result": 7, "id": "1"},
    // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
    // ]
    // responses底层的红黑树的每个节点中，真实存放的内容如下：
    // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
    // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
    responses[id] = response;
}

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
Json::Value BatchResponse::getResult(int id)
{
    Json::Value result;
    Json::Value i = id;
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
    getResult(i, result);
    return result;
}

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
void BatchResponse::getResult(Json::Value &id, Json::Value &result)
{
    if (getErrorCode(id) == 0)
    {
        result = responses[id];
    }
    else
    {
        result = Json::nullValue;
    }
}

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
int BatchResponse::getErrorCode(Json::Value &id)
{
    if (std::find(errorResponses.begin(), errorResponses.end(), id) !=
            errorResponses.end())
    {
        return responses[id]["code"].asInt();
    }
    return 0;
}

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
string BatchResponse::getErrorMessage(Json::Value &id)
{
    if (std::find(errorResponses.begin(), errorResponses.end(), id) !=
            errorResponses.end())
    {
        return responses[id]["message"].asString();
    }
    return "";
}

string BatchResponse::getErrorMessage(int id)
{
    Json::Value i = id;
    return getErrorMessage(i);
}

bool BatchResponse::hasErrors()
{
    // errorResponses的作用：存放，服务端程序返回给客户端的，json格式的响应对象数组中，具有"error"成员的响应对象的，id成员的值
    // 服务端程序返回给客户端的，json格式的响应对象数组，示例如下：
    // <-- [
    // {"jsonrpc": "2.0", "result": 7, "id": "1"},
    // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
    // ]
    // errorResponses底层的数组中，真实存放的内容如下：
    // [{"id": "1"}, {"id": "5"}]
    return !errorResponses.empty();
}
