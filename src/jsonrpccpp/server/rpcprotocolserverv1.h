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

        // ����4����������void HandleJsonRequest(const Json::Value &request, Json::Value &response)�������ڲ�ʵ��ʱ��ʹ�õĺ���
        bool ValidateRequestFields(const Json::Value &request);
        void WrapResult(const Json::Value &request, Json::Value &response, Json::Value &retValue);
        void WrapError(const Json::Value &request, int code, const std::string &message, Json::Value &result);
        void WrapException(const Json::Value &request, const JsonRpcException &exception, Json::Value &result);

        // ��ȡ�������ͣ��ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request��
        // ��һ��Զ�̷������󣬻���һ��Զ��֪ͨ����
        procedure_t GetRequestType(const Json::Value &request);

    };

} // namespace jsonrpc

#endif // JSONRPC_CPP_RPCPROTOCOLSERVERV1_H
