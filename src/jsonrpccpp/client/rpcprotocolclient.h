/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    responsehandler.h
 * @date    13.03.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef RESPONSEHANDLER_H
#define RESPONSEHANDLER_H

#include <jsonrpccpp/common/exception.h>
#include <jsonrpccpp/common/jsonparser.h>
#include <string>
#include "client.h"

namespace jsonrpc
{


    /**
     * @brief The RpcProtocolClient class handles the json-rpc 2.0 protocol for the client side.
     */
    // json-rpc v2.0�ͻ���Э��ʵ�֣�
    // 1).����RPC�������
    // 2).�Է���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���value�����з�����
    //      ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
    //      ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(value)��
    //          ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
    class RpcProtocolClient
    {
    public:
        RpcProtocolClient(clientVersion_t version = JSONRPC_CLIENT_V2);

        /**
         * @brief This method builds a valid json-rpc 2.0 request object based on passed parameters.
         * The id starts at 1 and is incremented for each request. To reset this value to one, call
         * the jsonrpc::RpcProRpcProtocolClient::resetId() method.
         * @param method - name of method or notification to be called
         * @param parameter - parameters represented as json objects
         * @return the string representation of the request to be built.
         */
        // �������ܣ�����RPC�������
        // �����������壺
        //      method���ͻ��˳�������ķ�����������������
        //      parameter�������ͻ��˳�������ķ������������Ĳ���
        //      result�������õ�json��ʽ��RPC������󣬴�ŵ�����
        //      isNotification��֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
        //                      �ò��������ڿ����Ƿ񹹽�һ��֪ͨ
        std::string BuildRequest(const std::string &method, const Json::Value &parameter, bool isNotification);

        /**
         * @brief BuildRequest does the same as std::string jsonrpc::RpcProRpcProtocolClient::BuildRequest(const std::string& method, const Json::Value& parameter);
         * The only difference here is that the result is returend by value, using the result parameter.
         * @param method - name of method or notification to be called
         * @param parameter - parameters represented as json objects
         * @param result - the string representation will be hold within this reference.
         */
        // �������ܣ�����RPC�������
        // �����������壺
        //      method���ͻ��˳�������ķ�����������������
        //      parameter�������ͻ��˳�������ķ������������Ĳ���
        //      result�������õ�json��ʽ��RPC������󣬴�ŵ�����
        //      isNotification��֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
        //                      �ò��������ڿ����Ƿ񹹽�һ��֪ͨ
        void BuildRequest(const std::string &method, const Json::Value &parameter, std::string &result, bool isNotification);

        /**
         * @brief Does the same as Json::Value RpcProtocolClient::HandleResponse(const std::string& response) throw(Exception)
         * but returns result as reference for performance speed up.
         */
        // �������ܣ��������˷��ظ��ͻ��˵�json��ʽ����Ӧ����
        // �����������壺
        //      response������˷��ظ��ͻ��˵���Ӧ����
        //      result����ŷ���˳��򷵻صĵ��ú�����ִ�н��
        void HandleResponse(const std::string &response, Json::Value &result) ;

        /**
         * @brief HandleResponse
         * @param response
         * @param result
         * @return response id
         */
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ�����ǰ������·�ʽ���ڷ���˳����н��й���ģ�
        // ������һ��rpc����ʱ����֪֮ͨ�⣬����˶�����ظ���Ӧ��
        // ��Ӧ��ʾΪһ��JSON����ʹ�����³�Ա��
        // jsonrpc
        //     ָ��JSON-RPCЭ��汾���ַ���������׼ȷдΪ��2.0��
        // result
        //     �ó�Ա��rpc���óɹ�ʱ������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����б�������ó�Ա��
        //     �����÷���˳��򷽷��������ʱ������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����в����԰����ó�Ա��
        //     ������еı����÷��������˸ó�Ա��ֵ��
        // error
        //     �ó�Ա��rpc����ʧ��ʱ������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����б�������ó�Ա��
        //     �ó�Ա��rpc���óɹ�ʱ������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����в����԰����ó�Ա��
        //     �ó�Ա�Ĳ���ֵ������Ϊ5.1�ж���Ĵ������
        // id
        //     �ó�Ա���������
        //     �ó�Աֵ��������������е�id��Աֵһ�¡�
        //     ����������������У�����������Ϣ����������������Ч���󣩣����ֵ����Ϊ��ֵ��
        //     ����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ�����У��������result��error��Ա����������Ա���ܱ�ͬʱ������
        // 5.1�������
        //     ��һ��rpc������������ʱ�����ص���Ӧ���������������Ա����������Ϊ�������г�Ա�����Ķ���
        // code
        //     ʹ����ֵ��ʾ���쳣�Ĵ������͡� ����Ϊ������
        // message
        //     �Ըô���ļ������ַ����� ������Ӧ�����޶��ڼ�̵�һ�仰��
        // data
        //     �������ڴ��󸽼���Ϣ�Ļ������ͻ�ṹ�����͡��ó�Ա�ɺ��ԡ� �ó�Աֵ�ɷ���˶��壨������ϸ�Ĵ�����Ϣ��Ƕ�׵Ĵ���ȣ���
        // ==========================================================================================================
        // ����������������ã����ǶԷ���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���value�����з�����
        // ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
        // ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(value)��
        //      ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
        // ==========================================================================================================
        Json::Value HandleResponse(const Json::Value &response, Json::Value &result) ;

        // �������
        static const std::string KEY_PROTOCOL_VERSION;
        static const std::string KEY_PROCEDURE_NAME;
        static const std::string KEY_ID;
        static const std::string KEY_PARAMETER;
        static const std::string KEY_AUTH;
        // ��Ӧ����
        static const std::string KEY_RESULT;
        static const std::string KEY_ERROR;
        // �������
        static const std::string KEY_ERROR_CODE;
        static const std::string KEY_ERROR_MESSAGE;
        static const std::string KEY_ERROR_DATA;

    private:
        // ��¼���ͻ��˰汾�����������ĸ��汾��json-rpc ��V1.0��V2.0��
        clientVersion_t version; 
        // �������ܣ�����ʵʩ����json��ʽ��RPC�������
        // �����������壺
        //      method���ͻ��˳�������ķ�����������������
        //      parameter�������ͻ��˳�������ķ������������Ĳ���
        //      result�������õ�json��ʽ��RPC������󣬴�ŵ�����
        //      isNotification��֪ͨ����������id����Ա��json��ʽ��RPC������󣬾���һ��֪ͨ
        //                      �ò��������ڿ����Ƿ񹹽�һ��֪ͨ
        void BuildRequest(int id, const std::string &method, const Json::Value &parameter, Json::Value &result, bool isNotification);
       
        // �жϣ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ�������һ����ȷ�����ã���json��ʽ����Ӧ���󣨻ظ���
        bool ValidateResponse(const Json::Value &response);
       
        // �жϣ�
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ�����У��Ƿ��С�error����Ա������:
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        bool HasError(const Json::Value &response);
       
        // ���÷�����в����ڵķ�����rpc����:
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
        // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // =================================================================================================================
        // ����������������ã����ǶԷ���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���response�����з�����
        // ��1����response�еģ�"error": {"code": -32601, "message": "Method not found"��"data": }�����ݣ���Ϊ�쳣��Ϣ���׳�
        // ��2����response�еģ�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
        // ��3����response�еģ�"error": {"code": -32601}�����ݣ���Ϊ�쳣��Ϣ���׳�
        // =================================================================================================================
        void throwErrorException(const Json::Value &response);
    };
}
#endif // RESPONSEHANDLER_H
