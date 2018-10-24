/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    abstractserverconnector.h
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_SERVERCONNECTOR_H_
#define JSONRPC_CPP_SERVERCONNECTOR_H_

#include "iclientconnectionhandler.h"
#include <string>

namespace jsonrpc
{
    // ��������������
    // ʵ�ַ���˳�������ͻ���������RPC��Ϣ��
    // ����ͻ��������Լ���RPC����Ĵ�����
    class AbstractServerConnector
    {
    public:
        AbstractServerConnector();
        virtual ~AbstractServerConnector();

        /**
         * This method should signal the Connector to start waiting for requests, in
         * any way that is appropriate for the derived connector class.
         * If something went wrong, this method should return false, otherwise true.
         */
        virtual bool StartListening() = 0;
        /**
         * This method should signal the Connector to stop waiting for requests, in
         * any way that is appropriate for the derived connector class.
         * If something went wrong, this method should return false, otherwise true.
         */
        virtual bool StopListening() = 0;
        
        // �����������壺
        //      request���ͻ��˳��������˳��򣬷��͵�RPC������Ϣ�����󣩣������request��
        //      response������˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ���󣬴�ŵ�response��
        // �������ܣ�
        //      �ͻ��˳��������˳��򣬷���request�д�ŵ�RPC������Ϣ�����󣩣�
        //      ��ʹ��response�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
        // Json-Rpc V2.0 Э��Զ�̹��̵���ʾ�����£�
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
        // ����˸��ͻ��˷��صģ�json��ʽ����Ӧ�������£�
        // <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
        void ProcessRequest(const std::string &request, std::string &response);
        
        // ���ã�����ͻ������Ӵ�����
        // ����ͻ������Ӵ�������
        // ��Ҫ����ʵ�ַ���˳�����ͻ��˳�������
        // RPC���󣬼���������������������ķ�������������
        // ִ�иú����������һ�����
        void SetHandler(IClientConnectionHandler *handler);
        // ��ȡ������ͻ������Ӵ�����
        IClientConnectionHandler *GetHandler();

    private:
        // =========================================================================================
        // IClientConnectionHandler ��Ĺ���
        // ����ͻ������Ӵ�������
        // ��Ҫ����ʵ�ַ���˳�����ͻ��˳�������
        // RPC���󣬼���������������������ķ�������������
        // ִ�иú����������һ�����
        // =========================================================================================
        // IClientConnectionHandler �������
        // ����࣬�ǣ���ģʽ�Ҳ࣬��㣬�������࣬
        // ��1������࣬ͳһ����ģʽ�Ҳ����е����ݣ��������˳�����
        // ���ԣ���ģʽ��ֻ࣬�����������һ��ָ�룬�Ϳ���ʵ��ͨ����һ��ָ�룬
        // ʹ����ģʽ�Ҳ����е����ݵ�Ч����
        // ��2��ͬʱ���������֮�µ����е��࣬�����δ�������仯�����������ģʽ��࣬����κ�Ӱ�죬
        // ���ʵ������ģʽ�����Ҳ�Ľ���
        // =========================================================================================
        IClientConnectionHandler *handler;
    };

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_ERVERCONNECTOR_H_ */
