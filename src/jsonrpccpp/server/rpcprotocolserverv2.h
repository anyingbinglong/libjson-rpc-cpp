/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    rpcprotocolserverv2.h
* @date    31.12.2012
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#ifndef JSONRPC_CPP_RPCPROTOCOLSERVERV2_H_
#define JSONRPC_CPP_RPCPROTOCOLSERVERV2_H_

#include <string>
#include <vector>
#include <map>

#include <jsonrpccpp/common/exception.h>
#include "abstractprotocolhandler.h"


#define KEY_REQUEST_VERSION     "jsonrpc"
#define JSON_RPC_VERSION2        "2.0"

namespace jsonrpc
{
    class RpcProtocolServerV2 : public AbstractProtocolHandler
    {
    public:
        RpcProtocolServerV2(IProcedureInvokationHandler &handler);

        // �����������壺
        //      req����1���ͻ��˳��������˳����͵�req�ǣ�������������
        //               �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�����������
        //           ��2���ͻ��˳��������˳����͵�req�ǣ���ͨ��������
        //               �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�������
        //      response����1���ͻ��˳��������˳����͵��ǣ���ͨ��������
        //                    ����˳��򷵻ظ��ͻ��˵�response�ǣ�json��ʽ����Ӧ��������
        //                ��2���ͻ��˳��������˳����͵��ǣ���ͨ��������
        //                    ����˳��򷵻ظ��ͻ��˵�response�ǣ�json��ʽ����Ӧ����
        // �������ܣ�
        // ʵ�֣���json-rpc v2.0Э���У��ͻ�����ͨ��������Ĵ���
        //      �Կͻ��˳��������˳����͵�req -- json��ʽ��RPC���������з����봦��
        //      ��������Ľ�� -- json��ʽ����Ӧ���󣬷ŵ�response��
        // ʵ�֣���json-rpc v2.0Э���У��ͻ���������������Ĵ���
        //      �Կͻ��˳��������˳����͵�req -- json��ʽ��RPC�������������з����봦��
        //      ��������Ľ�� -- json��ʽ����Ӧ�������飬�ŵ�response��
        void HandleJsonRequest(const Json::Value &request, Json::Value &response);

        // ����4����������void HandleJsonRequest(const Json::Value &request, Json::Value &response)�������ڲ�ʵ��ʱ��ʹ�õĺ���
        // Validate��ȷ�ϣ�֤ʵ
        // ����ͨ���У��ڽ��յ���ȷ��Ϣ����ȷ��Ϣ��������ͷ����͵�һ���ź�
        // ����˳���ȷ�ϣ���飩���ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request�У������ĸ����ֶ�ֵ���Ƿ�Ϸ�����������Json-Rpc v2.0Э��Ĺ淶��
        bool ValidateRequestFields(const Json::Value &val);

        // ===================================================================================================================
        // �����������壺
        // request���ͻ��������˷��͵ģ�json��ʽ���������
        // response������˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ����
        // result������˳���ִ�У��ͻ��������˷��͵ģ�json��ʽ����������еģ�"method"�ֶδ�ŵĺ����ģ�������
        // ===================================================================================================================
        // ����������⣺
        // Wrap������
        // ���������������rpc����:
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": "1"}
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
        // <-- {"jsonrpc": "2.0", "result": 19, "id": "1"}
        // ===================================================================================================================
        // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ����ȷ����Ӧ����response���ظ�����
        // ʹ��request��result���й��죬����Ľ������ŵ�response��
        // ��ȷ����Ӧ����response���ظ�����{"jsonrpc": "2.0", "result": 19, "id": "1"}
        // ===================================================================================================================
        void WrapResult(const Json::Value &request, Json::Value &response, Json::Value &retValue);

        // ===================================================================================================================
        // Wrap������
        // ���÷�����в����ڵķ�����rpc����:
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
        // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // ===================================================================================================================
        // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result������Ľ������ŵ�result��
        // ʹ��request, code��message���й��죬����Ľ������ŵ�result��
        // ��1����һ�ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // ��2���ڶ��ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": null}
        // ===================================================================================================================
        void WrapError(const Json::Value &request, int code, const std::string &message, Json::Value &result);

        // ===================================================================================================================
        // Wrap������
        // ���÷�����в����ڵķ�����rpc����:
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
        // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
        // ========================================================================================================================
        // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ�Ĵ������result������Ľ������ŵ�result��
        // ʹ��request��exception���й��죬����Ľ������ŵ�result��
        // ��1����һ�ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": "1"}
        // ��2���ڶ��ִ������{"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found", "data": ""}, "id": null}
        // ========================================================================================================================
        void WrapException(const Json::Value &request, const JsonRpcException &exception, Json::Value &result);

        // ��ȡ�������ͣ��ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request��
        // ��һ��Զ�̷������󣬻���һ��Զ��֪ͨ����
        procedure_t GetRequestType(const Json::Value &request);

    private:
        // �����������壺
        //      req���ͻ��˳��������˳����͵�req�ǣ���ͨ��������
        //           �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�������
        //      response���ͻ��˳��������˳����͵��ǣ���ͨ��������
        //                ����˳��򷵻ظ��ͻ��˵�response�ǣ�json��ʽ����Ӧ����
        // �������ܣ�
        // ʵ�֣���json-rpc v2.0Э���У��ͻ�����ͨ��������Ĵ���
        // Json-Rpc V2.0 Э����ͨԶ�̹��̵���ʾ�����£�
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
        // ����˸��ͻ��˷��صģ�json��ʽ����Ӧ�������£�
        // <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
        void HandleSingleRequest(const Json::Value &request, Json::Value &response);

        // �����������壺
        //      req���ͻ��˳��������˳����͵�req�ǣ�������������
        //               �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�����������
        //      response���ͻ��˳��������˳����͵��ǣ���ͨ��������
        //                    ����˳��򷵻ظ��ͻ��˵�response�ǣ�json��ʽ����Ӧ��������
        // �������ܣ�
        // ʵ�֣���json-rpc v2.0Э���У��ͻ���������������Ĵ���
        // ʲô���������ã�
        // ����Ҫͬʱ���Ͷ���������ʱ���ͻ��˿��Է���һ���������������������顣
        // ���������õ�����������������ʱ�����������Ҫ����һ��������ͻ��˷��͵���������������Ӧ����Ӧ�������顣
        // ��Ӧ���������е�ÿ����Ӧ������������������е�ÿ���������һһ��Ӧ��������֪ͨ���͵��������
        // ����˿��Բ����ģ�������˳��������ȵĲ����ԣ���������Щ�������á�
        // ���ڣ���Щ���Ӧ����Ӧ���󣬿����������˳�򣬰����ڷ��ص���Ӧ���������У����ԣ��ͻ�����Ҫʹ����Ӧ���������и�����Ӧ�����е�id��Ա����ƥ����֮��Ӧ���������
        // ��������ʾ����
        // �ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
        // --> [
        // {"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},
        // {"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},
        // {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
        // {"foo": "boo"},
        // {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},
        // {"jsonrpc": "2.0", "method": "get_data", "id": "9"}
        // ]
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "result": 19, "id": "2"},
        // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"},
        // {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
        // ]
        void HandleBatchRequest(const Json::Value &requests, Json::Value &response);
    };

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_RPCPROTOCOLSERVERV2_H_ */
