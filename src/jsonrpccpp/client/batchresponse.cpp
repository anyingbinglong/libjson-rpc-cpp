/*************************************************************************
* libjson-rpc-cpp
*************************************************************************
* @file    batchresponse.cpp
* @date    10/9/2014
* @author  Peter Spiess-Knafl <dev@spiessknafl.at>
* @license See attached LICENSE.txt
************************************************************************/

#include "batchresponse.h"
#include <algorithm>

using namespace jsonrpc;
using namespace std;

BatchResponse::BatchResponse() {}

// �����������壺
//      id�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ�����id��Ա
//      response�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ����ľ������ݣ���ŵ�������
//      isError����ʶ�������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У������ӵ���Ӧ����ľ��������У��Ƿ����"error"��Ա
// �������ܣ�
//      �����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����һ����Ӧ����
// ==================================================================================
// �����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����һ����Ӧ����ʾ����
// ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
// <-- [
// {"jsonrpc": "2.0", "result": 7, "id": "1"},
// {"jsonrpc": "2.0", "result": 19, "id": "2"},
// {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
// ]
// ==================================================================================
// �������json��ʽ��RPC��Ӧ���������У���������һ����Ӧ����
// {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
// ==================================================================================
// ������ɺ󣬷���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
// <-- [
// {"jsonrpc": "2.0", "result": 7, "id": "1"},
// {"jsonrpc": "2.0", "result": 19, "id": "2"},
// {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": null},
// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"},
// {"jsonrpc": "2.0", "result": ["hello", 5], "id": "9"}
// ]
// ==================================================================================
void BatchResponse::addResponse(Json::Value &id, Json::Value response,
                                bool isError)
{
    if (isError)
    {
        // errorResponses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ����ģ�id��Ա��ֵ
        // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
        // <-- [
        // {"jsonrpc": "2.0", "result": 7, "id": "1"},
        // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
        // ]
        // errorResponses�ײ�������У���ʵ��ŵ��������£�
        // [{"id": "1"}, {"id": "5"}]
        errorResponses.push_back(id);
    }
    // responses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ��������
    // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
    // <-- [
    // {"jsonrpc": "2.0", "result": 7, "id": "1"},
    // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
    // ]
    // responses�ײ�ĺ������ÿ���ڵ��У���ʵ��ŵ��������£�
    // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
    // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
    responses[id] = response;
}

// �����������壺��Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����ģ�id��Ա��ֵ
// ����ֵ���壺��Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�����ֵ
// �������ܣ�
// responses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ��������
// ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
// <-- [
// {"jsonrpc": "2.0", "result": 7, "id": "1"},
// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
// ]
// responses�ײ�ĺ������ÿ���ڵ��У���ʵ��ŵ��������£�
// [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
// [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
// =========================================================================================================
// �ú������ܣ���responses�У���ȡһ������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����
// =========================================================================================================
Json::Value BatchResponse::getResult(int id)
{
    Json::Value result;
    Json::Value i = id;
    // �����������壺
    //      ��1��id����Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����ģ�id��Ա��ֵ
    //      ��2��result����Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�����ֵ����ŵ�������
    // �������ܣ�
    // responses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ��������
    // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
    // <-- [
    // {"jsonrpc": "2.0", "result": 7, "id": "1"},
    // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
    // ]
    // responses�ײ�ĺ������ÿ���ڵ��У���ʵ��ŵ��������£�
    // [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
    // [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
    // =========================================================================================================
    // �ú������ܣ���responses�У���ȡһ������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����
    // =========================================================================================================
    getResult(i, result);
    return result;
}

// �����������壺
//      ��1��id����Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����ģ�id��Ա��ֵ
//      ��2��result����Ҫ��ȡ�ķ���˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�����ֵ����ŵ�������
// �������ܣ�
// responses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ��������
// ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
// <-- [
// {"jsonrpc": "2.0", "result": 7, "id": "1"},
// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
// ]
// responses�ײ�ĺ������ÿ���ڵ��У���ʵ��ŵ��������£�
// [{"id": "1"}, {"jsonrpc": "2.0", "result": 7, "id": "1"}]
// [{"id": "5"}, {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}]
// =========================================================================================================
// �ú������ܣ���responses�У���ȡһ������˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ����
// =========================================================================================================
void BatchResponse::getResult(Json::Value &id, Json::Value &result)
{
    if (getErrorCode(id) == 0)
    {
        result = responses[id];
    }
    else
    {
        result = Json::nullValue;
    }
}

// �����������壺����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����id��Ա��ֵ
// ��������ֵ���壺����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����еģ�"code"��Ա��ֵ
// �������ܣ�
// ��ȡ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����еģ�"code"��Ա��ֵ
// �����rpc��������ʾ��:
// �ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
// --> [
// {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
// {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"}
// ]
// ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
// <-- [
// {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": "2"},
// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
// ]
int BatchResponse::getErrorCode(Json::Value &id)
{
    if (std::find(errorResponses.begin(), errorResponses.end(), id) !=
            errorResponses.end())
    {
        return responses[id]["code"].asInt();
    }
    return 0;
}

// �����������壺����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����id��Ա��ֵ
// ��������ֵ���壺����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����еģ�"message"��Ա��ֵ
// �������ܣ�
// ��ȡ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ�����еģ�"message"��Ա��ֵ
// �����rpc��������ʾ��:
// �ͻ��˳��������˳����͵ģ�json��ʽ��RPC����������飬����:
// --> [
// {"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},
// {"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"}
// ]
// ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬���£�
// <-- [
// {"jsonrpc": "2.0", "error": {"code": -32600, "message": "Invalid Request"}, "id": "2"},
// {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
// ]
string BatchResponse::getErrorMessage(Json::Value &id)
{
    if (std::find(errorResponses.begin(), errorResponses.end(), id) !=
            errorResponses.end())
    {
        return responses[id]["message"].asString();
    }
    return "";
}

string BatchResponse::getErrorMessage(int id)
{
    Json::Value i = id;
    return getErrorMessage(i);
}

bool BatchResponse::hasErrors()
{
    // errorResponses�����ã���ţ�����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ���������У�����"error"��Ա����Ӧ����ģ�id��Ա��ֵ
    // ����˳��򷵻ظ��ͻ��˵ģ�json��ʽ����Ӧ�������飬ʾ�����£�
    // <-- [
    // {"jsonrpc": "2.0", "result": 7, "id": "1"},
    // {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "5"}
    // ]
    // errorResponses�ײ�������У���ʵ��ŵ��������£�
    // [{"id": "1"}, {"id": "5"}]
    return !errorResponses.empty();
}
