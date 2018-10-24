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
    // ��[ptr, size * nmemb]�е��������ݣ����Ƶ�[s->ptr + s->len, ������]�н��д��
    // ����ʵ�ֿͻ��˽��̣��յ�����˽��̷��͹�����HttpӦ�����ݵı���
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

// �������ܣ�
// �ͻ��˳��������˳��򣬷���message�д�ŵ�RPC������Ϣ�����󣩣�      
// ��ʹ��result�����գ�����˳��򣬷��ظ��ͻ��˳����json��ʽ����Ӧ����
void HttpClient::SendRPCMessage(const std::string &message,
                                std::string &result) {
    // ����Ҫ���ʵ�url
    curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
    // ���ûص�����writefunc��
    // �ú�������libcurl���յ����ݣ��ͻ��˽��̣��յ�����˽��̷��͹�����HttpӦ�����ݣ��󱻵��ã�
    // ��ˣ��ú��������������ݱ��湦��
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);

    CURLcode res;
    
    // libcurl���յ����ݣ��ͻ��˽��̣��յ�����˽��̷��͹�����HttpӦ�����ݣ��󣬵��ûص�����writefunc��
    // �ڸú����ڲ���ʵ�ֽ��յ������ݣ��ŵ�struct string s���н��б���
    struct string s;
    init_string(&s);
    
    // begin�������µ�Http������Ϣͷ��
    // ������ͷ������messageһ�𣬷��͸�����������
    struct curl_slist *headers = NULL;
    for (std::map<std::string, std::string>::iterator header = this->headers.begin(); header != this->headers.end(); ++header)
    {
        headers = curl_slist_append(
                      headers, (header->first + ": " + header->second).c_str());
    }
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    // end���������
    
    // ָ��POST���ݣ�Ϊ message.c_str() --> �ͻ��˽��̣����͸�������Ľ��̵���Ϣ
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message.c_str());
    // ���ã�libcurl���յ����ݣ��ͻ��˽��̣��յ�����˽��̷��͹�����HttpӦ�����ݣ��󣬵��ûص�����writefunc��
    // �ڸú����ڲ���ʵ�ֽ��յ������ݣ��ŵ�struct string s���н��б���
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    // �޸�Http������Ϣͷ
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 
    // ����Http���������ѵ����ʱ��
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);

    res = curl_easy_perform(curl);// ��� curl_easy_setoptָ�������е�ѡ��

    result = s.ptr;// �ͻ��˽��̣��յ�����˽��̷��͹�����HttpӦ������s.ptr���ŵ�result�б���
    
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
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);// CURLINFO_RESPONSE_CODE����ȡHttpӦ��ͷ��Ϣ�е�Ӧ����

    if (http_code != 200) {
        throw JsonRpcException(Errors::ERROR_RPC_INTERNAL_ERROR, result);
    }
}

// ����Http�ͻ��˳�����Ҫ���ӵķ���˳����Url
void HttpClient::SetUrl(const std::string &url) { this->url = url; }

// ����Http���������ѵ����ʱ��
void HttpClient::SetTimeout(long timeout) { this->timeout = timeout; }

// ����Http����ͷ
void HttpClient::AddHeader(const std::string &attr, const std::string &val) {
    this->headers[attr] = val;
}

// ɾ��Http����ͷ
void HttpClient::RemoveHeader(const std::string &attr) {
    this->headers.erase(attr);
}
