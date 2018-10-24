/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    rpcprotocolserverv1.h
 * @date    10/23/2014
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_RPCPROTOCOLSERVERV1_H
#define JSONRPC_CPP_RPCPROTOCOLSERVERV1_H

#include <jsonrpccpp/common/exception.h>
#include "abstractprotocolhandler.h"

namespace jsonrpc
{

    class RpcProtocolServerV1 : public AbstractProtocolHandler
    {
    public:
        RpcProtocolServerV1(IProcedureInvokationHandler &handler);

        void HandleJsonRequest(const Json::Value &request, Json::Value &response);

        // 以下4个函数，是void HandleJsonRequest(const Json::Value &request, Json::Value &response)函数，内部实现时，使用的函数
        bool ValidateRequestFields(const Json::Value &request);
        void WrapResult(const Json::Value &request, Json::Value &response, Json::Value &retValue);
        void WrapError(const Json::Value &request, int code, const std::string &message, Json::Value &result);
        void WrapException(const Json::Value &request, const JsonRpcException &exception, Json::Value &result);

        // 获取请求类型：客户端程序，发送给服务端程序的Json格式的请求对象request，
        // 是一个远程方法请求，还是一个远程通知请求
        procedure_t GetRequestType(const Json::Value &request);

    };

} // namespace jsonrpc

#endif // JSONRPC_CPP_RPCPROTOCOLSERVERV1_H
