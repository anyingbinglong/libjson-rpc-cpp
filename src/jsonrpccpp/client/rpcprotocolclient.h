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
    // json-rpc v2.0客户端协议实现：
    // 1).构建RPC请求对象
    // 2).对服务端程序，返回给客户端程序的json格式的响应对象（回复）value，进行分析，
    //      （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
    //      （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(value)，
    //          将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
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
        // 函数功能：构建RPC请求对象
        // 函数参数含义：
        //      method：客户端程序请求的方法（函数）的名称
        //      parameter：传给客户端程序请求的方法（函数）的参数
        //      result：构建好的json格式的RPC请求对象，存放到这里
        //      isNotification：通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
        //                      该参数，用于控制是否构建一个通知
        std::string BuildRequest(const std::string &method, const Json::Value &parameter, bool isNotification);

        /**
         * @brief BuildRequest does the same as std::string jsonrpc::RpcProRpcProtocolClient::BuildRequest(const std::string& method, const Json::Value& parameter);
         * The only difference here is that the result is returend by value, using the result parameter.
         * @param method - name of method or notification to be called
         * @param parameter - parameters represented as json objects
         * @param result - the string representation will be hold within this reference.
         */
        // 函数功能：构建RPC请求对象
        // 函数参数含义：
        //      method：客户端程序请求的方法（函数）的名称
        //      parameter：传给客户端程序请求的方法（函数）的参数
        //      result：构建好的json格式的RPC请求对象，存放到这里
        //      isNotification：通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
        //                      该参数，用于控制是否构建一个通知
        void BuildRequest(const std::string &method, const Json::Value &parameter, std::string &result, bool isNotification);

        /**
         * @brief Does the same as Json::Value RpcProtocolClient::HandleResponse(const std::string& response) throw(Exception)
         * but returns result as reference for performance speed up.
         */
        // 函数功能：处理服务端返回给客户端的json格式的响应对象
        // 函数参数含义：
        //      response：服务端返回给客户端的响应对象
        //      result：存放服务端程序返回的调用函数的执行结果
        void HandleResponse(const std::string &response, Json::Value &result) ;

        /**
         * @brief HandleResponse
         * @param response
         * @param result
         * @return response id
         */
        // 服务端返回给客户端的，json格式的响应对象，是按照如下方式，在服务端程序中进行构造的：
        // 当发起一个rpc调用时，除通知之外，服务端都必须回复响应。
        // 响应表示为一个JSON对象，使用以下成员：
        // jsonrpc
        //     指定JSON-RPC协议版本的字符串，必须准确写为“2.0”
        // result
        //     该成员在rpc调用成功时，服务端程序，返回给客户端程序的json格式的响应对象中必须包含该成员。
        //     当调用服务端程序方法引起错误时，服务端程序，返回给客户端程序的json格式的响应对象中不可以包含该成员。
        //     服务端中的被调用方法决定了该成员的值。
        // error
        //     该成员在rpc调用失败时，服务端程序，返回给客户端程序的json格式的响应对象中必须包含该成员。
        //     该成员在rpc调用成功时，服务端程序，返回给客户端程序的json格式的响应对象中不可以包含该成员。
        //     该成员的参数值，必须为5.1中定义的错误对象。
        // id
        //     该成员必须包含。
        //     该成员值必须与请求对象中的id成员值一致。
        //     若检查出，请求对象中，包含错误信息（例如参数错误或无效请求），则该值必须为空值。
        //     服务端程序，返回给客户端程序的json格式的响应对象中，必须包含result或error成员，但两个成员不能被同时包含。
        // 5.1错误对象
        //     当一个rpc调用遇到错误时，返回的响应对象必须包含错误成员参数，并且为带有下列成员参数的对象：
        // code
        //     使用数值表示该异常的错误类型。 必须为整数。
        // message
        //     对该错误的简单描述字符串。 该描述应尽量限定在简短的一句话。
        // data
        //     包含关于错误附加信息的基本类型或结构化类型。该成员可忽略。 该成员值由服务端定义（例如详细的错误信息，嵌套的错误等）。
        // ==========================================================================================================
        // 下面这个函数的作用，就是对服务端程序，返回给客户端程序的json格式的响应对象（回复）value，进行分析，
        // （1）在客户端程序，向服务端程序，发起的，rpc调用成功时，将服务端程序返回的调用函数的执行结果，存放到result中
        // （2）在客户端程序，向服务端程序，发起的，rpc调用失败时，执行this->throwErrorException(value)，
        //      将value中的"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
        // ==========================================================================================================
        Json::Value HandleResponse(const Json::Value &response, Json::Value &result) ;

        // 请求对象
        static const std::string KEY_PROTOCOL_VERSION;
        static const std::string KEY_PROCEDURE_NAME;
        static const std::string KEY_ID;
        static const std::string KEY_PARAMETER;
        static const std::string KEY_AUTH;
        // 响应对象
        static const std::string KEY_RESULT;
        static const std::string KEY_ERROR;
        // 错误对象
        static const std::string KEY_ERROR_CODE;
        static const std::string KEY_ERROR_MESSAGE;
        static const std::string KEY_ERROR_DATA;

    private:
        // 记录：客户端版本，表明，是哪个版本的json-rpc （V1.0或V2.0）
        clientVersion_t version; 
        // 函数功能：真正实施构建json格式的RPC请求对象
        // 函数参数含义：
        //      method：客户端程序请求的方法（函数）的名称
        //      parameter：传给客户端程序请求的方法（函数）的参数
        //      result：构建好的json格式的RPC请求对象，存放到这里
        //      isNotification：通知：不包含“id”成员的json格式的RPC请求对象，就是一个通知
        //                      该参数，用于控制是否构建一个通知
        void BuildRequest(int id, const std::string &method, const Json::Value &parameter, Json::Value &result, bool isNotification);
       
        // 判断：服务端程序，返回给客户端程序的json格式的响应对象（回复），是一个正确（可用）的json格式的响应对象（回复）
        bool ValidateResponse(const Json::Value &response);
       
        // 判断：
        // 服务端返回给客户端的，json格式的响应对象中，是否有“error”成员，例如:
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        bool HasError(const Json::Value &response);
       
        // 调用服务端中不存在的方法的rpc调用:
        // 客户端向服务端发送的，json格式的RPC请求对象，如下:
        // --> {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
        // 服务端返回给客户端的，json格式的响应对象，如下:
        // <-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
        // =================================================================================================================
        // 下面这个函数的作用，就是对服务端程序，返回给客户端程序的json格式的响应对象（回复）response，进行分析，
        // （1）将response中的，"error": {"code": -32601, "message": "Method not found"，"data": }，内容，作为异常信息，抛出
        // （2）将response中的，"error": {"code": -32601, "message": "Method not found"}，内容，作为异常信息，抛出
        // （3）将response中的，"error": {"code": -32601}，内容，作为异常信息，抛出
        // =================================================================================================================
        void throwErrorException(const Json::Value &response);
    };
}
#endif // RESPONSEHANDLER_H
