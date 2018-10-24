/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    abstractserver.h
 * @date    30.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_ABSTRACTSERVER_H_
#define JSONRPC_CPP_ABSTRACTSERVER_H_

#include <map>
#include <string>
#include <vector>
#include <jsonrpccpp/common/procedure.h>
#include "abstractserverconnector.h"
#include "iprocedureinvokationhandler.h"
#include "iclientconnectionhandler.h"
#include "requesthandlerfactory.h"

namespace jsonrpc
{
    template<class S>
    class AbstractServer : public IProcedureInvokationHandler
    {
        public:
            typedef void(S::*methodPointer_t)       (const Json::Value &parameter, Json::Value &result);
            typedef void(S::*notificationPointer_t) (const Json::Value &parameter);

            AbstractServer(AbstractServerConnector &connector, serverVersion_t type = JSONRPC_SERVER_V2) :
                connection(connector)
            {
                this->handler = RequestHandlerFactory::createProtocolHandler(type, *this);
                connector.SetHandler(this->handler);
            }

            virtual ~AbstractServer()
            {
                delete this->handler;
            }

            bool StartListening()
            {
                return connection.StartListening();
            }

            bool StopListening()
            {
                return connection.StopListening();
            }

            virtual void HandleMethodCall(Procedure &proc, const Json::Value& input, Json::Value& output)
            {
                S* instance = dynamic_cast<S*>(this);
                (instance->*methods[proc.GetProcedureName()])(input, output);
            }

            virtual void HandleNotificationCall(Procedure &proc, const Json::Value& input)
            {
                S* instance = dynamic_cast<S*>(this);
                (instance->*notifications[proc.GetProcedureName()])(input);
            }

        protected:
            // ��Զ�̷���pointer�����浽����˳����Զ�̷����б�methods�У��Թ��ͻ��˳�����е���
            bool bindAndAddMethod(const Procedure& proc, methodPointer_t pointer)
            {
                // ��1��Զ�̹���proc�����ͣ�ΪRPC_METHODԶ�̷�����
                // ��2������˳����Զ�̷����б�methods
                //     ��
                //     ����˳����Զ��֪ͨ�б�notifications�У�
                //     ��������Ҫ�����Զ�̹���proc
                if(proc.GetProcedureType() == RPC_METHOD && !this->symbolExists(proc.GetProcedureName()))
                {
                    // Զ�̹��̵ĺ��壺
                    // ��1��Զ�̷���
                    // ��2��Զ��֪ͨ
                    // ��Զ�̹���proc�����浽����˳����Զ�̹����б�procedures�У��Թ��ͻ��˳�����е���
                    this->handler->AddProcedure(proc);
                    // ��Զ�̷���pointer�����浽����˳����Զ�̷����б�methods�У��Թ��ͻ��˳�����е���
                    this->methods[proc.GetProcedureName()] = pointer;
                    return true;
                }
                return false;
            }
            // ��Զ��֪ͨpointer�����浽����˳����Զ��֪ͨ�б�notifications�У��Թ��ͻ��˳�����е���
            bool bindAndAddNotification(const Procedure& proc, notificationPointer_t pointer)
            {
                // ��1��Զ�̹���proc�����ͣ�ΪRPC_NOTIFICATIONԶ��֪ͨ��
                // ��2������˳����Զ�̷����б�methods
                //     ��
                //     ����˳����Զ��֪ͨ�б�notifications�У�
                //     ��������Ҫ�����Զ�̹���proc
                if(proc.GetProcedureType() == RPC_NOTIFICATION && !this->symbolExists(proc.GetProcedureName()))
                {
                    // ��1��Զ�̷���
                    // ��2��Զ��֪ͨ
                    // ��Զ�̹���proc�����浽����˳����Զ�̹����б�procedures�У��Թ��ͻ��˳�����е���
                    this->handler->AddProcedure(proc);
                    // ��Զ�̷���pointer�����浽����˳����Զ�̷����б�methods�У��Թ��ͻ��˳�����е���
                    this->notifications[proc.GetProcedureName()] = pointer;
                    return true;
                }
                return false;
            }

        private:
            AbstractServerConnector                         &connection;
            // Json-Rpc V1.0 �� V2.0Э�飬����˴�����
            IProtocolHandler                                *handler;
            // ����˳����Զ�̷����б�methods���������й��ͻ��˳�����е��õ�Զ�̷���
            std::map<std::string, methodPointer_t>          methods;
            // ����˳����Զ��֪ͨ�б�notifications���������й��ͻ��˳�����е��õ�Զ��֪ͨ
            std::map<std::string, notificationPointer_t>    notifications;
            
            // ��������name���壺����˳����Զ�̷��������ֻ��߷���˳����Զ��֪ͨ������
            // �ж�name����
            // ����˳����Զ�̷���methods
            // ��
            // ����˳����Զ��֪ͨ�б�notifications�У�
            // �Ƿ����
            bool symbolExists(const std::string &name)
            {
                if (methods.find(name) != methods.end())
                    return true;// ����
                if (notifications.find(name) != notifications.end())
                    return true;// ����
                return false;// ������
            }
    };

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_ABSTRACTSERVER_H_ */
