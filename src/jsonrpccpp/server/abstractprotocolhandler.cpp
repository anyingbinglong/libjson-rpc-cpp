/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    abstractprotocolhandler.cpp
 * @date    10/23/2014
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include "abstractprotocolhandler.h"
#include <jsonrpccpp/common/errors.h>
#include <jsonrpccpp/common/jsonparser.h>

#include <map>

using namespace jsonrpc;
using namespace std;

AbstractProtocolHandler::AbstractProtocolHandler(
    IProcedureInvokationHandler &handler)
    : handler(handler) {}

AbstractProtocolHandler::~AbstractProtocolHandler() {}

// 函数参数procedure的含义：
// 服务端程序的远程过程列表procedures，保存所有供客户端程序进行调用的远程过程
// 远程过程的含义：
// （1）远程方法
// （2）远程通知
// 函数功能：
// 向服务端程序的远程过程列表procedures中，添加一个远程过程
void AbstractProtocolHandler::AddProcedure(const Procedure &procedure)
{
    this->procedures[procedure.GetProcedureName()] = procedure;
}

// 函数参数含义：
//      request：客户端程序向服务端程序发送的json格式的RPC请求对象（数组）
//      retValue：对客户端程序向服务端程序发送的request 
//                -- json格式的RPC请求对象（数组）进行分析与处理的结果 -- json格式的响应对象（数组），放到retValue中
// =============================================================================================================
// 函数功能
// 实现：对json-rpc v2.0协议中，客户端RPC远程过程调用请求的处理
// 对客户端程序向服务端程序发送的request -- json格式的RPC请求对象（数组）进行分析与处理，
// 并将处理的结果 -- json格式的响应对象（数组），放到retValue中
// =============================================================================================================
// 模板方法设计模式中，稳定的算法骨架，需要在父类class AbstractProtocolHandler （也就是这个类）中进行实现，
// 下面这个函数，内部，使用 HandleJsonRequest(const Json::Value& request, Json::Value& response)函数进行实现
// =============================================================================================================
void AbstractProtocolHandler::HandleRequest(const std::string &request, std::string &retValue)
{
    Json::Reader reader;// 使用Json::Reader来解析请求字符串request为Json格式
    Json::Value req;// 将Json::Reader解析请求字符串request为Json格式，的结果，存放到req中
    Json::Value resp;
    Json::FastWriter w;

    if (reader.parse(request, req, false))// 解析请求字符串request为Json格式，并将结果，存放到req中
    {
        // 函数参数含义：
        //      req：（1）客户端程序向服务端程序发送的req是：批量调用请求
        //               客户端程序向服务端程序发送的req是：json格式的RPC请求对象数组
        //           （2）客户端程序向服务端程序发送的req是：普通调用请求
        //               客户端程序向服务端程序发送的req是：json格式的RPC请求对象
        //      response：（1）客户端程序向服务端程序发送的是：普通调用请求
        //                    服务端程序返回给客户端的resp是：json格式的响应对象数组
        //                （2）客户端程序向服务端程序发送的是：普通调用请求
        //                    服务端程序返回给客户端的resp是：json格式的响应对象
        // =======================================================================================
        // 函数功能：
        // 实现：对json-rpc v2.0协议中，客户端普通调用请求的处理
        //      对客户端程序向服务端程序发送的req -- json格式的RPC请求对象进行分析与处理，
        //      并将处理的结果 -- json格式的响应对象，放到resp中
        // 实现：对json-rpc v2.0协议中，客户端批量调用请求的处理
        //      对客户端程序向服务端程序发送的req -- json格式的RPC请求对象数组进行分析与处理，
        //      并将处理的结果 -- json格式的响应对象数组，放到resp中
        // =======================================================================================
        // 下面这个纯虚函数，是：模板方法设计模式中，实现上面稳定的算法骨架的，步骤函数，
        // 需要在子类class RpcProtocolServerV1和class RpcProtocolServerV2中进行实现
        // 此处调用的也是，子类子类class RpcProtocolServerV1和class RpcProtocolServerV2中实现的函数
        // =======================================================================================
        this->HandleJsonRequest(req, resp);
    }
    else
    {
        this->WrapError(Json::nullValue, Errors::ERROR_RPC_JSON_PARSE_ERROR,
                        Errors::GetErrorMessage(Errors::ERROR_RPC_JSON_PARSE_ERROR),
                        resp);
    }

    if (resp != Json::nullValue)
    {
        retValue = w.write(resp);// 将resp变成不排版的json格式字符串，用于在网络上传输，减少传输数据量
    }
}

// 函数参数含义：
//      request：客户端程序向服务端程序发送的json格式的RPC请求对象（数组）
//      response：对客户端程序向服务端程序发送的request 
//                -- json格式的RPC请求对象（数组）进行分析与处理的结果 -- json格式的响应对象（数组），放到response中
// =============================================================================================================
// 函数功能
// 实现：对json-rpc v2.0协议中，客户端RPC远程过程调用请求的处理
// 对客户端程序向服务端程序发送的request -- json格式的RPC请求对象（数组）进行分析与处理，
// 并将处理的结果 -- json格式的响应对象（数组），放到response中
// =============================================================================================================
void AbstractProtocolHandler::ProcessRequest(const Json::Value &request, Json::Value &response)
{
    // 服务端程序的远程过程列表procedures，保存所有供客户端程序进行调用的远程过程
    // 远程过程的含义：
    // （1）远程方法
    // （2）远程通知
    // 在服务端程序的远程过程列表procedures中，获取（查找）客户端程序发来的请求对象request中的所包含的远程过程的名字
    Procedure &method = this->procedures[request[KEY_REQUEST_METHODNAME].asString()];
    
    // 在客户端程序，向服务端程序，发起的，rpc调用成功时，
    // 将服务端程序，调用远程过程的执行结果，存放到result中
    Json::Value result;
    
    // 客户端程序发送给服务端程序请求对象request的内容，是远程方法调用
    if (method.GetProcedureType() == RPC_METHOD)
    {
        // 处理远程方法调用：
        // （1）服务端程序真正执行，客户端程序发来的请求对象request中的所包含的远程方法method( request[KEY_REQUEST_PARAMETERS] )
        // （2）将服务端程序，调用远程方法method的执行结果，存放到result中
        handler.HandleMethodCall(method, request[KEY_REQUEST_PARAMETERS], result);
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
        this->WrapResult(request, response, result);
    }
    else// 客户端程序发送给服务端程序请求对象request的内容，是远程通知调用
    {
        // 处理远程通知调用：
        // 服务端程序真正执行，客户端程序发来的请求对象request中的所包含的远程通知method( request[KEY_REQUEST_PARAMETERS] )
        handler.HandleNotificationCall(method, request[KEY_REQUEST_PARAMETERS]);
        response = Json::nullValue;
    }
}

// 函数参数含义：
//      request：客户端程序向服务端程序发送的json格式的RPC请求对象（数组）
// ===============================================================================================================
// 函数功能
// Validate：确认，证实
// 网络通信中，在接收到正确信息或正确信息分组后，向发送方回送的一个信号
// 服务端程序，确认（检查），客户端程序，发送给服务端程序的Json格式的请求对象request中，包含的各个字段值，
// 是否合法（即：符合Json-Rpc v2.0协议的规范）
// ===============================================================================================================
int AbstractProtocolHandler::ValidateRequest(const Json::Value &request)
{
    int error = 0;
    Procedure proc;
    // Validate：确认，证实
    // 网络通信中，在接收到正确信息或正确信息分组后，向发送方回送的一个信号
    // 服务端程序，确认（检查），客户端程序，发送给服务端程序的Json格式的请求对象request中，包含的各个字段值，是否合法（即：符合Json-Rpc v2.0协议的规范）
    // Json-Rpc V2.0 协议普通远程过程调用示例如下：
    // 客户端向服务端发送的，json格式的RPC请求对象，如下:
    // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
    // 服务端给客户端返回的，json格式的响应对象，如下：
    // <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
    if (!this->ValidateRequestFields(request))
    {
        error = Errors::ERROR_RPC_INVALID_REQUEST;
    }
    else
    {
        // 服务端程序的远程过程列表procedures，保存所有供客户端程序进行调用的远程过程
        // 远程过程的含义：
        // （1）远程方法
        // （2）远程通知
        // 在服务端程序的远程过程列表procedures中，查找客户端程序发来的请求对象request中的所包含的远程过程的名字
        map<string, Procedure>::iterator it =
            this->procedures.find(request[KEY_REQUEST_METHODNAME].asString());
        // 以下内容，就是对客户端程序发来的请求对象request中的内容，进行验证
        if (it != this->procedures.end())
        {
            proc = it->second;
            if (this->GetRequestType(request) == RPC_METHOD &&
                    proc.GetProcedureType() == RPC_NOTIFICATION)
            {
                error = Errors::ERROR_SERVER_PROCEDURE_IS_NOTIFICATION;
            }
            else if (this->GetRequestType(request) == RPC_NOTIFICATION &&
                     proc.GetProcedureType() == RPC_METHOD)
            {
                error = Errors::ERROR_SERVER_PROCEDURE_IS_METHOD;
            }
            else if (!proc.ValdiateParameters(request[KEY_REQUEST_PARAMETERS]))
            {
                error = Errors::ERROR_RPC_INVALID_PARAMS;
            }
        }
        else
        {
            error = Errors::ERROR_RPC_METHOD_NOT_FOUND;
        }
    }
    return error;
}
