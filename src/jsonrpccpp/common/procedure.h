/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    procedure.h
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_PROCEDURE_H_
#define JSONRPC_CPP_PROCEDURE_H_

#include <string>
#include <map>

#include "jsonparser.h"
#include "specification.h"

namespace jsonrpc
{
    typedef std::map<std::string, jsontype_t> parameterNameList_t;
    typedef std::vector<jsontype_t> parameterPositionList_t;

    typedef enum {PARAMS_BY_NAME, PARAMS_BY_POSITION} parameterDeclaration_t;
    // 这个类，是用来保存（管理）服务端程序提供给客户端程序使用的远程过程（函数）的信息的
    class Procedure
    {
    public:

        Procedure();

        /**
         * @brief Constructor for notificaiton with parameters as va_list. The last parameter must be NULL.
         * If no parameters are passed, parameters either do not exist, or cannot be checked for type compliance by the library.
         * @param name
         */
        // 通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
        // 没有包含“id”成员的请求对象为通知， 作为通知的请求对象表明客户端对相应的响应对象并不感兴趣，服务端也没有响应对象需要返回给客户端。
        // 服务端不能回复一个单独的通知，也不能回复那些包含在批量请求中的通知。
        // 由于通知没有服务端返回的响应对象，所以通知是否被定义是无法确定的。同样，客户端不会意识到有任何错误发生（例如参数缺省，内部错误）。
        // 通知:
        // 客户端向服务端发送的，json格式的RPC请求对象，如下:
        // --> {"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]}
        // 服务端不会给客户端返回一个json格式的响应对象
        Procedure(const std::string &name, parameterDeclaration_t paramType, ...);

        /**
         * @brief Constructor for method with parameters as va_list. The last parameter must be NULL.
         * If no parameters are passed, parameters either do not exist, or cannot be checked for type compliance by the library.
         * @param name
         * @param returntype
         */
        // 调用服务端中不存在的方法的rpc调用:
        // 客户端向服务端发送的，json格式的RPC请求对象，如下:
        // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
        // 服务端返回给客户端的，json格式的响应对象，如下:
        // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        Procedure(const std::string &name, parameterDeclaration_t paramType, jsontype_t returntype, ...);


        /**
         * This method is validating the incoming parameters for each procedure.
         * @param parameters - should contain the parameter-object of an valid json-rpc 2.0 request
         * @see http://groups.google.com/group/json-rpc/web/json-rpc-2-0
         * @return true on successful validation false otherwise.
         *
         * If the valid parameters are of Type JSON_ARRAY or JSON_OBJECT, they can only be checked for name and not for their structure.
         */
        bool ValdiateParameters(const Json::Value &parameters) const;


        //Various get methods.
        const parameterNameList_t& GetParameters() const;
        procedure_t GetProcedureType() const;
        const std::string& GetProcedureName() const;
        jsontype_t GetReturnType() const;
        parameterDeclaration_t GetParameterDeclarationType() const;

        //Various set methods.
        void SetProcedureName(const std::string &name);
        void SetProcedureType(procedure_t type);
        void SetReturnType(jsontype_t type);
        void SetParameterDeclarationType(parameterDeclaration_t type);


        /**
         * @brief AddParameter
         * @param name describes the name of the parameter. In case of an positional parameters, this value can be anything.
         * @param type describes the defined type for this parameter.
         */
        void AddParameter(const std::string &name, jsontype_t type);

        bool ValidateNamedParameters(const Json::Value &parameters) const;
        bool ValidatePositionalParameters(const Json::Value &parameters) const;

    private:

        /**
         * Each Procedure should have a name.
         */
        std::string                 procedureName;

        /**
         * This map represents all necessary Parameters of each Procedure.
         * The string represents the name of each parameter and JsonType the type it should have.
         */
        parameterNameList_t         parametersName;

        /**
         * This vector holds all parametertypes by position.
         */
        parameterPositionList_t     parametersPosition;

        /**
         * @brief defines whether the procedure is a method or a notification
         */
        procedure_t                 procedureType;

        /**
         * @brief this field is only valid if procedure is of type method (not notification).
         */
        jsontype_t                  returntype;

        /**
         * @brief paramDeclaration this field defines if procedure uses named or positional parameters.
         */
        parameterDeclaration_t      paramDeclaration;

        bool ValidateSingleParameter(jsontype_t expectedType, const Json::Value &value) const;
    };
} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_PROCEDURE_H_ */
