/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    httpclient.cpp
* @date    02.01.2013
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#include "httpclient.h"
#include <cstdlib>
#include <curl/curl.h>
#include <string.h>
#include <string>

#include <iostream>

using namespace jsonrpc;

class curl_initializer {
public:
    curl_initializer() { curl_global_init(CURL_GLOBAL_ALL); }
    ~curl_initializer() { curl_global_cleanup(); }
};

// See here: http://curl.haxx.se/libcurl/c/curl_global_init.html
static curl_initializer _curl_init = curl_initializer();

/**
 * taken from
 * http://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
 */
struct string {
    char *ptr;
    size_t len;
};

static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = (char *)realloc(s->ptr, new_len + 1);
    // 将[ptr, size * nmemb]中的数据内容，复制到[s->ptr + s->len, 。。。]中进行存放
    // 即：实现客户端进程，收到服务端进程发送过来的Http应答数据的保存
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size * nmemb;
}

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = static_cast<char *>(malloc(s->len + 1));
    s->ptr[0] = '\0';
}

HttpClient::HttpClient(const std::string &url) : url(url) {
    this->timeout = 10000;
    curl = curl_easy_init();
}

HttpClient::~HttpClient() { curl_easy_cleanup(curl); }

// 函数功能：
// 客户端程序向服务端程序，发送message中存放的RPC请求消息（对象），      
// 并使用result，接收，服务端程序，返回给客户端程序的json格式的响应对象
void HttpClient::SendRPCMessage(const std::string &message,
                                std::string &result) {
    // 设置要访问的url
    curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
    // 设置回调函数writefunc，
    // 该函数将在libcurl接收到数据（客户端进程，收到服务端进程发送过来的Http应答数据）后被调用，
    // 因此，该函数被用于做数据保存功能
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);

    CURLcode res;
    
    // libcurl接收到数据（客户端进程，收到服务端进程发送过来的Http应答数据）后，调用回调函数writefunc，
    // 在该函数内部，实现将收到的数据，放到struct string s；中进行保存
    struct string s;
    init_string(&s);
    
    // begin：制作新的Http请求消息头，
    // 该请求头，将与message一起，发送给服务器进程
    struct curl_slist *headers = NULL;
    for (std::map<std::string, std::string>::iterator header = this->headers.begin(); header != this->headers.end(); ++header)
    {
        headers = curl_slist_append(
                      headers, (header->first + ": " + header->second).c_str());
    }
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    // end：制作完成
    
    // 指定POST内容，为 message.c_str() --> 客户端进程，发送给，服务的进程的消息
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message.c_str());
    // 设置：libcurl接收到数据（客户端进程，收到服务端进程发送过来的Http应答数据）后，调用回调函数writefunc，
    // 在该函数内部，实现将收到的数据，放到struct string s；中进行保存
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    // 修改Http请求消息头
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 
    // 设置Http请求所花费的最大时间
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);

    res = curl_easy_perform(curl);// 完成 curl_easy_setopt指定的所有的选项

    result = s.ptr;// 客户端进程，收到服务端进程发送过来的Http应答数据s.ptr，放到result中保存
    
    free(s.ptr);
    curl_slist_free_all(headers);
    if (res != CURLE_OK) {
        std::stringstream str;
        str << "libcurl error: " << res;

        if (res == 7)
            str << " -> Could not connect to " << this->url;
        else if (res == 28)
            str << " -> Operation timed out";
        throw JsonRpcException(Errors::ERROR_CLIENT_CONNECTOR, str.str());
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);// CURLINFO_RESPONSE_CODE：获取Http应答头消息中的应答码

    if (http_code != 200) {
        throw JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR, result);
    }
}

// 设置Http客户端程序想要连接的服务端程序的Url
void HttpClient::SetUrl(const std::string &url) { this->url = url; }

// 设置Http请求所花费的最大时间
void HttpClient::SetTimeout(long timeout) { this->timeout = timeout; }

// 增加Http请求头
void HttpClient::AddHeader(const std::string &attr, const std::string &val) {
    this->headers[attr] = val;
}

// 删除Http请求头
void HttpClient::RemoveHeader(const std::string &attr) {
    this->headers.erase(attr);
}
