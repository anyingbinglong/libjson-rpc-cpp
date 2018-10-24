/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    client.h
 * @date    03.01.2013
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_CLIENT_H_
#define JSONRPC_CPP_CLIENT_H_

#include "iclientconnector.h"
#include "batchcall.h"
#include "batchresponse.h"
#include <jsonrpccpp/common/jsonparser.h>

#include <vector>
#include <map>

namespace jsonrpc
{
    class RpcProtocolClient;
    // json-rpc Э��Ŀͻ��˳���İ汾 v1.0 �� v2.0
    typedef enum {JSONRPC_CLIENT_V1, JSONRPC_CLIENT_V2} clientVersion_t;
    
    // json-rpc v1.0�� v2.0�ͻ��˳���ʵ��
    class Client
    {
    public:
        Client(IClientConnector &connector, clientVersion_t version = JSONRPC_CLIENT_V2);
        virtual ~Client();
        
        // �����������壺
        //      name���ͻ��˳�������ķ�����������������
        //      parameter�������ͻ��˳�������ķ������������Ĳ���
        //      result���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
        // �������ܣ�����Զ�̷���
        //      1).����RPC�������
        //      2).�ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ�����󣩣�      
        //      ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
        //      3).�Է���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���response�����з�����
        //          ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
        //          ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(response)��
        //              ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�  
        void CallMethod(const std::string &name, const Json::Value &parameter, Json::Value &result);
        // �����������壺
        //      name���ͻ��˳�������ķ�����������������
        //      parameter�������ͻ��˳�������ķ������������Ĳ���
        // ��������ֵ�ĺ��壺
        //      �ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ��˸÷���ֵ��
        // �������ܣ�����Զ�̷���
        Json::Value CallMethod(const std::string &name, const Json::Value &parameter);

        // Զ�̹��̵ĺ��壺
        // ��1��Զ�̷���
        // ��2��Զ��֪ͨ
        // ����Զ�̹���
        void CallProcedures(const BatchCall &calls, BatchResponse &response);
        BatchResponse CallProcedures(const BatchCall &calls) ;

        // �����������壺
        //      name���ͻ��˳��������Զ��֪ͨ��������������
        //      parameter�������ͻ��˳��������Զ��֪ͨ���������Ĳ���
        // �������ܣ�����Զ��֪ͨ
        //      1).����RPC�������
        //      2).�ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ�����󣩣�     
        //         ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
        void CallNotification(const std::string &name, const Json::Value &parameter);

    private:
        // ����ͻ��˳����������������������������RPC��Ϣ�������շ��������򷵻صĴ�����
        IClientConnector  &connector;
        
        // RpcProtocolClient�ĺ��壺
        // json-rpc v2.0�ͻ���Э��ʵ�֣�
        // 1).����RPC�������
        // 2).�Է���˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣨻ظ���value�����з�����
        //      ��1���ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��������˳��򷵻صĵ��ú�����ִ�н������ŵ�result��
        //      ��2���ڿͻ��˳��������˳��򣬷���ģ�rpc����ʧ��ʱ��ִ��this->throwErrorException(value)��
        //          ��value�е�"error": {"code": -32601, "message": "Method not found"}�����ݣ���Ϊ�쳣��Ϣ���׳�
        // ��ϸ���ݣ����Բο�rpcprotocolclient.h�ļ����ݣ��������
        RpcProtocolClient *protocol;
    };

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_CLIENT_H_ */
