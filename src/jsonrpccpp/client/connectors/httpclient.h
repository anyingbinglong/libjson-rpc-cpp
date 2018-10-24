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
    
    // �������ܣ�
    // �ͻ��˳��������˳��򣬷���message�д�ŵ�RPC������Ϣ�����󣩣�      
    // ��ʹ��result�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
    virtual void SendRPCMessage(const std::string &message, std::string &result);
    
    // ����Http�ͻ��˳�����Ҫ���ӵķ���˳����Url
    void SetUrl(const std::string &url);
    
    // ����Http���������ѵ����ʱ��
    void SetTimeout(long timeout);
    
    // ����Http����ͷ
    void AddHeader(const std::string &attr, const std::string &val);
    // ɾ��Http����ͷ
    void RemoveHeader(const std::string &attr);

private:
    // ���棺Http����ͷ
    std::map<std::string, std::string> headers;
    // ���棺Http�ͻ��˳�����Ҫ���ӵķ���˳����Url
    std::string url;

    /**
     * @brief timeout for http request in milliseconds
     */
    long timeout;
    CURL *curl;
};

} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_HTTPCLIENT_H_ */
