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
    // ʵ�֣�json-rpc v2.0Э���У��ͻ���������Ӧ���ظ���
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
    // ============================================================================
    // �������Ҫʵ�ֵĹ��ܣ����ǹ���������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ��������
    // ============================================================================
    // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
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
        // �����������壺
        //      id�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ�����id��Ա
        //      response�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ����ľ������ݣ���ŵ�������
        //      isError����ʶ�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ����ľ��������У��Ƿ����"error"��Ա
        // �������ܣ�
        //      �����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����һ����Ӧ����
        // ==================================================================================
        // �����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����һ����Ӧ����ʾ����
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "result": 19, "id": "2"},
        // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // ==================================================================================
        // �������json��ʽ��RPC��Ӧ���������У���������һ����Ӧ����
        // {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
        // ==================================================================================
        // ������ɺ󣬷���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
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
        // �����������壺��Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����ģ�id��Ա��ֵ
        // ����ֵ���壺��Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�����ֵ
        // �������ܣ�
        // responses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ��������
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // responses�ײ�ĺ������ÿ���ڵ��У���ʵ��ŵ��������£�
        // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
        // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
        // =========================================================================================================
        // �ú������ܣ���responses�У���ȡһ������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����
        // =========================================================================================================
        Json::Value getResult(int id);

        // �����������壺
        //      ��1��id����Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����ģ�id��Ա��ֵ
        //      ��2��result����Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�����ֵ����ŵ�������
        // �������ܣ�
        // responses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ��������
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // responses�ײ�ĺ������ÿ���ڵ��У���ʵ��ŵ��������£�
        // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
        // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
        // =========================================================================================================
        // �ú������ܣ���responses�У���ȡһ������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����
        // =========================================================================================================
        void getResult(Json::Value &id, Json::Value &result);

        /**
         * @brief getErrorCode method checks if for a given id, an error occurred in the batch request.
         * @param id
         */
        // �����������壺����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����id��Ա��ֵ
        // ��������ֵ���壺����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����еģ�"code"��Ա��ֵ
        // �������ܣ�
        // ��ȡ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����еģ�"code"��Ա��ֵ
        // �����rpc��������ʾ��:
        // �ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
        // --> [
        // {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
        // {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"}
        // ]
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
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
        // �����������壺����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����id��Ա��ֵ
        // ��������ֵ���壺����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����еģ�"message"��Ա��ֵ
        // �������ܣ�
        // ��ȡ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����еģ�"message"��Ա��ֵ
        // �����rpc��������ʾ��:
        // �ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
        // --> [
        // {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
        // {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"}
        // ]
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
        // <-- [
        // {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": "2"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        std::string getErrorMessage(Json::Value &id);
        std::string getErrorMessage(int id);

        bool hasErrors();

    private:
        // responses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ��������
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // responses�ײ�ĺ������ÿ���ڵ��У���ʵ��ŵ��������£�
        // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
        // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
        std::map<Json::Value, Json::Value> responses;
        
        // errorResponses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ����ģ�id��Ա��ֵ
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // errorResponses�ײ�������У���ʵ��ŵ��������£�
        // [{"id": "1"}, {"id": "5"}]
        std::vector<Json::Value> errorResponses;

    };

} // namespace jsonrpc

#endif // JSONRPC_BATCHRESPONSE_H
