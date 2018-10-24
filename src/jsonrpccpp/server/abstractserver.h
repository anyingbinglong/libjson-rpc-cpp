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
            // 将远程方法pointer，保存到服务端程序的远程方法列表methods中，以供客户端程序进行调用
            bool bindAndAddMethod(const Procedure& proc, methodPointer_t pointer)
            {
                // （1）远程过程proc的类型，为RPC_METHOD远程方法，
                // （2）服务端程序的远程方法列表methods
                //     和
                //     服务端程序的远程通知列表notifications中，
                //     不存在需要保存的远程过程proc
                if(proc.GetProcedureType() == RPC_METHOD && !this->symbolExists(proc.GetProcedureName()))
                {
                    // 远程过程的含义：
                    // （1）远程方法
                    // （2）远程通知
                    // 将远程过程proc，保存到服务端程序的远程过程列表procedures中，以供客户端程序进行调用
                    this->handler->AddProcedure(proc);
                    // 将远程方法pointer，保存到服务端程序的远程方法列表methods中，以供客户端程序进行调用
                    this->methods[proc.GetProcedureName()] = pointer;
                    return true;
                }
                return false;
            }
            // 将远程通知pointer，保存到服务端程序的远程通知列表notifications中，以供客户端程序进行调用
            bool bindAndAddNotification(const Procedure& proc, notificationPointer_t pointer)
            {
                // （1）远程过程proc的类型，为RPC_NOTIFICATION远程通知，
                // （2）服务端程序的远程方法列表methods
                //     和
                //     服务端程序的远程通知列表notifications中，
                //     不存在需要保存的远程过程proc
                if(proc.GetProcedureType() == RPC_NOTIFICATION && !this->symbolExists(proc.GetProcedureName()))
                {
                    // （1）远程方法
                    // （2）远程通知
                    // 将远程过程proc，保存到服务端程序的远程过程列表procedures中，以供客户端程序进行调用
                    this->handler->AddProcedure(proc);
                    // 将远程方法pointer，保存到服务端程序的远程方法列表methods中，以供客户端程序进行调用
                    this->notifications[proc.GetProcedureName()] = pointer;
                    return true;
                }
                return false;
            }

        private:
            AbstractServerConnector                         &connection;
            // Json-Rpc V1.0 和 V2.0协议，服务端处理者
            IProtocolHandler                                *handler;
            // 服务端程序的远程方法列表methods，保存所有供客户端程序进行调用的远程方法
            std::map<std::string, methodPointer_t>          methods;
            // 服务端程序的远程通知列表notifications，保存所有供客户端程序进行调用的远程通知
            std::map<std::string, notificationPointer_t>    notifications;
            
            // 函数参数name含义：服务端程序的远程方法的名字或者服务端程序的远程通知的名字
            // 判断name，在
            // 服务端程序的远程方法methods
            // 和
            // 服务端程序的远程通知列表notifications中，
            // 是否存在
            bool symbolExists(const std::string &name)
            {
                if (methods.find(name) != methods.end())
                    return true;// 存在
                if (notifications.find(name) != notifications.end())
                    return true;// 存在
                return false;// 不存在
            }
    };

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_ABSTRACTSERVER_H_ */
