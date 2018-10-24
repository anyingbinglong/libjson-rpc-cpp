/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    abstractprotocolhandler.h
 * @date    10/23/2014
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_ABSTRACTPROTOCOLHANDLER_H
#define JSONRPC_CPP_ABSTRACTPROTOCOLHANDLER_H

#include "iprocedureinvokationhandler.h"
#include "iclientconnectionhandler.h"
#include <map>
#include <string>
#include <jsonrpccpp/common/procedure.h>

#define KEY_REQUEST_METHODNAME  "method"
#define KEY_REQUEST_ID          "id"
#define KEY_REQUEST_PARAMETERS  "params"
#define KEY_RESPONSE_ERROR      "error"
#define KEY_RESPONSE_RESULT     "result"

namespace jsonrpc
{
    // 这个类，使用了模板方法设计模式进行实现
    class AbstractProtocolHandler : public IProtocolHandler
    {
    public:
        AbstractProtocolHandler(IProcedureInvokationHandler &handler);
        virtual ~AbstractProtocolHandler();

        // 函数参数procedure的含义：
        // 服务端程序的远程过程列表procedures，保存所有供客户端程序进行调用的远程过程
        // 远程过程的含义：
        // （1）远程方法
        // （2）远程通知
        // 函数功能：
        // 向服务端程序的远程过程列表procedures中，添加一个远程过程
        virtual void AddProcedure(const Procedure &procedure);

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
        void HandleRequest(const std::string &request, std::string &retValue);

        // 下面5个纯虚函数，是：模板方法设计模式中，实现上面稳定的算法骨架的，步骤函数，需要在子类class RpcProtocolServerV1和class RpcProtocolServerV2中进行实现
        // 函数参数含义：
        //      req：（1）客户端程序向服务端程序发送的req是：批量调用请求
        //               客户端程序向服务端程序发送的req是：json格式的RPC请求对象数组
        //           （2）客户端程序向服务端程序发送的req是：普通调用请求
        //               客户端程序向服务端程序发送的req是：json格式的RPC请求对象
        //      response：（1）客户端程序向服务端程序发送的是：普通调用请求
        //                    服务端程序返回给客户端的response是：json格式的响应对象数组
        //                （2）客户端程序向服务端程序发送的是：普通调用请求
        //                    服务端程序返回给客户端的response是：json格式的响应对象
        // 函数功能：
        // 实现：对json-rpc v2.0协议中，客户端普通调用请求的处理
        //      对客户端程序向服务端程序发送的req -- json格式的RPC请求对象进行分析与处理，
        //      并将处理的结果 -- json格式的响应对象，放到response中
        // 实现：对json-rpc v2.0协议中，客户端批量调用请求的处理
        //      对客户端程序向服务端程序发送的req -- json格式的RPC请求对象数组进行分析与处理，
        //      并将处理的结果 -- json格式的响应对象数组，放到response中
        virtual void HandleJsonRequest(const Json::Value &request, Json::Value &response) = 0;

        // 以下3个函数，是void HandleJsonRequest(const Json::Value &request, Json::Value &response)函数，内部实现时，使用的函数
        // Validate：确认，证实
        // 网络通信中，在接收到正确信息或正确信息分组后，向发送方回送的一个信号
        // 服务端程序，确认（检查），客户端程序，发送给服务端程序的Json格式的请求对象request中，包含的各个字段值，是否合法（即：符合Json-Rpc v2.0协议的规范）
        virtual bool ValidateRequestFields(const Json::Value &val) = 0;

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
        virtual void WrapResult(const Json::Value &request, Json::Value &response, Json::Value &retValue) = 0;

        // ===================================================================================================================
        // Wrap：返回
        // 调用服务端中不存在的方法的rpc调用:
        // 客户端向服务端发送的，json格式的RPC请求对象，如下:
        // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
        // 服务端返回给客户端的，json格式的响应对象，如下:
        // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // ===================================================================================================================
        // 下面这个函数的作用，就是，构造，服务端程序，返回给客户端程序的json格式的错误对象result，构造的结果，存放到result中
        // 使用request, code和message进行构造，构造的结果，存放到result中
        // （1）第一种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // （2）第二种错误对象：{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": null}
        // ===================================================================================================================
        virtual void WrapError(const Json::Value &request, int code, const std::string &message, Json::Value &result) = 0;

        // 获取请求类型：客户端程序，发送给服务端程序的Json格式的请求对象request，
        // 是一个方法请求，还是一个通知请求
        virtual procedure_t GetRequestType(const Json::Value &request) = 0;

    protected:
        // 远程过程调用处理者
        // 远程过程的含义：
        // （1）远程方法
        // （2）远程通知
        IProcedureInvokationHandler &handler;
        
        // 服务端程序的远程过程列表procedures，保存所有供客户端程序进行调用的远程过程
        // 远程过程的含义：
        // （1）远程方法
        // （2）远程通知
        std::map<std::string, Procedure> procedures;

        // ===============================================================================================================
        // 模板方法设计模式中，稳定的算法骨架，需要在父类class AbstractProtocolHandler中进行实现
        // 下面这2个函数，在实现时，同时使用了上面的5个纯虚函数中的某几个，和，handler与procedures这2个对象对应的类的内部的成员函数
        // ===============================================================================================================
        
        // 函数参数含义：
        //      request：客户端程序向服务端程序发送的json格式的RPC请求对象（数组）
        //      response：对客户端程序向服务端程序发送的request
        //                -- json格式的RPC请求对象（数组）进行分析与处理的结果 -- json格式的响应对象（数组），放到response中
        // ===============================================================================================================
        // 函数功能
        // 实现：对json-rpc v2.0协议中，客户端RPC远程过程调用请求的处理
        // 对客户端程序向服务端程序发送的request -- json格式的RPC请求对象（数组）进行分析与处理，
        // 并将处理的结果 -- json格式的响应对象（数组），放到response中
        // ===============================================================================================================
        void ProcessRequest(const Json::Value &request, Json::Value &retValue);

        // 函数参数含义：
        //      request：客户端程序向服务端程序发送的json格式的RPC请求对象（数组）
        // ===============================================================================================================
        // 函数功能
        // Validate：确认，证实
        // 网络通信中，在接收到正确信息或正确信息分组后，向发送方回送的一个信号
        // 服务端程序，确认（检查），客户端程序，发送给服务端程序的Json格式的请求对象request中，包含的各个字段值，
        // 是否合法（即：符合Json-Rpc v2.0协议的规范）
        // ===============================================================================================================
        int ValidateRequest(const Json::Value &val);
    };

} // namespace jsonrpc

#endif // JSONRPC_CPP_ABSTRACTPROTOCOLHANDLER_H
