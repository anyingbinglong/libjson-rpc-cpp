/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    httpclient.h
* @date    02.01.2013
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#ifndef JSONRPC_CPP_HTTPCLIENT_H_
#define JSONRPC_CPP_HTTPCLIENT_H_

#include "../iclientconnector.h"
#include <curl/curl.h>
#include <jsonrpccpp/common/exception.h>
#include <map>

namespace jsonrpc {
class HttpClient : public IClientConnector {
public:
    HttpClient(const std::string &url);
    virtual ~HttpClient();
    
    // 函数功能：
    // 客户端程序向服务端程序，发送message中存放的RPC请求消息（对象），      
    // 并使用result，接收，服务端程序，返回给客户端程序的json格式的响应对象
    virtual void SendRPCMessage(const std::string &message, std::string &result);
    
    // 设置Http客户端程序想要连接的服务端程序的Url
    void SetUrl(const std::string &url);
    
    // 设置Http请求所花费的最大时间
    void SetTimeout(long timeout);
    
    // 增加Http请求头
    void AddHeader(const std::string &attr, const std::string &val);
    // 删除Http请求头
    void RemoveHeader(const std::string &attr);

private:
    // 保存：Http请求头
    std::map<std::string, std::string> headers;
    // 保存：Http客户端程序想要连接的服务端程序的Url
    std::string url;

    /**
     * @brief timeout for http request in milliseconds
     */
    long timeout;
    CURL *curl;
};

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_HTTPCLIENT_H_ */
