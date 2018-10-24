/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    abstractprotocolhandler.cpp
 * @date    10/23/2014
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include "abstractprotocolhandler.h"
#include <jsonrpccpp/common/errors.h>
#include <jsonrpccpp/common/jsonparser.h>

#include <map>

using namespace jsonrpc;
using namespace std;

AbstractProtocolHandler::AbstractProtocolHandler(
    IProcedureInvokationHandler &handler)
    : handler(handler) {}

AbstractProtocolHandler::~AbstractProtocolHandler() {}

// ��������procedure�ĺ��壺
// ����˳����Զ�̹����б�procedures���������й��ͻ��˳�����е��õ�Զ�̹���
// Զ�̹��̵ĺ��壺
// ��1��Զ�̷���
// ��2��Զ��֪ͨ
// �������ܣ�
// �����˳����Զ�̹����б�procedures�У����һ��Զ�̹���
void AbstractProtocolHandler::AddProcedure(const Procedure &procedure)
{
    this->procedures[procedure.GetProcedureName()] = procedure;
}

// �����������壺
//      request���ͻ��˳��������˳����͵�json��ʽ��RPC����������飩
//      retValue���Կͻ��˳��������˳����͵�request 
//                -- json��ʽ��RPC����������飩���з����봦��Ľ�� -- json��ʽ����Ӧ�������飩���ŵ�retValue��
// =============================================================================================================
// ��������
// ʵ�֣���json-rpc v2.0Э���У��ͻ���RPCԶ�̹��̵�������Ĵ���
// �Կͻ��˳��������˳����͵�request -- json��ʽ��RPC����������飩���з����봦��
// ��������Ľ�� -- json��ʽ����Ӧ�������飩���ŵ�retValue��
// =============================================================================================================
// ģ�巽�����ģʽ�У��ȶ����㷨�Ǽܣ���Ҫ�ڸ���class AbstractProtocolHandler ��Ҳ��������ࣩ�н���ʵ�֣�
// ��������������ڲ���ʹ�� HandleJsonRequest(const Json::Value& request, Json::Value& response)��������ʵ��
// =============================================================================================================
void AbstractProtocolHandler::HandleRequest(const std::string &request, std::string &retValue)
{
    Json::Reader reader;// ʹ��Json::Reader�����������ַ���requestΪJson��ʽ
    Json::Value req;// ��Json::Reader���������ַ���requestΪJson��ʽ���Ľ������ŵ�req��
    Json::Value resp;
    Json::FastWriter w;

    if (reader.parse(request, req, false))// ���������ַ���requestΪJson��ʽ�������������ŵ�req��
    {
        // �����������壺
        //      req����1���ͻ��˳��������˳����͵�req�ǣ�������������
        //               �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�����������
        //           ��2���ͻ��˳��������˳����͵�req�ǣ���ͨ��������
        //               �ͻ��˳��������˳����͵�req�ǣ�json��ʽ��RPC�������
        //      response����1���ͻ��˳��������˳����͵��ǣ���ͨ��������
        //                    ����˳��򷵻ظ��ͻ��˵�resp�ǣ�json��ʽ����Ӧ��������
        //                ��2���ͻ��˳��������˳����͵��ǣ���ͨ��������
        //                    ����˳��򷵻ظ��ͻ��˵�resp�ǣ�json��ʽ����Ӧ����
        // =======================================================================================
        // �������ܣ�
        // ʵ�֣���json-rpc v2.0Э���У��ͻ�����ͨ��������Ĵ���
        //      �Կͻ��˳��������˳����͵�req -- json��ʽ��RPC���������з����봦��
        //      ��������Ľ�� -- json��ʽ����Ӧ���󣬷ŵ�resp��
        // ʵ�֣���json-rpc v2.0Э���У��ͻ���������������Ĵ���
        //      �Կͻ��˳��������˳����͵�req -- json��ʽ��RPC�������������з����봦��
        //      ��������Ľ�� -- json��ʽ����Ӧ�������飬�ŵ�resp��
        // =======================================================================================
        // ����������麯�����ǣ�ģ�巽�����ģʽ�У�ʵ�������ȶ����㷨�Ǽܵģ����躯����
        // ��Ҫ������class RpcProtocolServerV1��class RpcProtocolServerV2�н���ʵ��
        // �˴����õ�Ҳ�ǣ���������class RpcProtocolServerV1��class RpcProtocolServerV2��ʵ�ֵĺ���
        // =======================================================================================
        this->HandleJsonRequest(req, resp);
    }
    else
    {
        this->WrapError(Json::nullValue, Errors::ERROR_RPC_JSON_PARSE_ERROR,
                        Errors::GetErrorMessage(Errors::ERROR_RPC_JSON_PARSE_ERROR),
                        resp);
    }

    if (resp != Json::nullValue)
    {
        retValue = w.write(resp);// ��resp��ɲ��Ű��json��ʽ�ַ����������������ϴ��䣬���ٴ���������
    }
}

// �����������壺
//      request���ͻ��˳��������˳����͵�json��ʽ��RPC����������飩
//      response���Կͻ��˳��������˳����͵�request 
//                -- json��ʽ��RPC����������飩���з����봦��Ľ�� -- json��ʽ����Ӧ�������飩���ŵ�response��
// =============================================================================================================
// ��������
// ʵ�֣���json-rpc v2.0Э���У��ͻ���RPCԶ�̹��̵�������Ĵ���
// �Կͻ��˳��������˳����͵�request -- json��ʽ��RPC����������飩���з����봦��
// ��������Ľ�� -- json��ʽ����Ӧ�������飩���ŵ�response��
// =============================================================================================================
void AbstractProtocolHandler::ProcessRequest(const Json::Value &request, Json::Value &response)
{
    // ����˳����Զ�̹����б�procedures���������й��ͻ��˳�����е��õ�Զ�̹���
    // Զ�̹��̵ĺ��壺
    // ��1��Զ�̷���
    // ��2��Զ��֪ͨ
    // �ڷ���˳����Զ�̹����б�procedures�У���ȡ�����ң��ͻ��˳��������������request�е���������Զ�̹��̵�����
    Procedure &method = this->procedures[request[KEY_REQUEST_METHODNAME].asString()];
    
    // �ڿͻ��˳��������˳��򣬷���ģ�rpc���óɹ�ʱ��
    // ������˳��򣬵���Զ�̹��̵�ִ�н������ŵ�result��
    Json::Value result;
    
    // �ͻ��˳����͸�����˳����������request�����ݣ���Զ�̷�������
    if (method.GetProcedureType() == RPC_METHOD)
    {
        // ����Զ�̷������ã�
        // ��1������˳�������ִ�У��ͻ��˳��������������request�е���������Զ�̷���method( request[KEY_REQUEST_PARAMETERS] )
        // ��2��������˳��򣬵���Զ�̷���method��ִ�н������ŵ�result��
        handler.HandleMethodCall(method, request[KEY_REQUEST_PARAMETERS], result);
        // ===================================================================================================================
        // �����������壺
        // request���ͻ��������˷��͵ģ�json��ʽ���������
        // response������˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ����
        // result������˳���ִ�У��ͻ��������˷��͵ģ�json��ʽ����������еģ�"method"�ֶδ�ŵĺ����ģ�������
        // ===================================================================================================================
        // ����������⣺
        // Wrap������
        // ���������������rpc����:
        // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
        // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": "1"}
        // ����˷��ظ��ͻ��˵ģ�json��ʽ����Ӧ��������:
        // <-- {"jsonrpc": "2.0", "result": 19, "id": "1"}
        // ===================================================================================================================
        // ����������������ã����ǣ����죬����˳��򣬷��ظ��ͻ��˳����json��ʽ����ȷ����Ӧ����response���ظ�����
        // ʹ��request��result���й��죬����Ľ������ŵ�response��
        // ��ȷ����Ӧ����response���ظ�����{"jsonrpc": "2.0", "result": 19, "id": "1"}
        // ===================================================================================================================
        this->WrapResult(request, response, result);
    }
    else// �ͻ��˳����͸�����˳����������request�����ݣ���Զ��֪ͨ����
    {
        // ����Զ��֪ͨ���ã�
        // ����˳�������ִ�У��ͻ��˳��������������request�е���������Զ��֪ͨmethod( request[KEY_REQUEST_PARAMETERS] )
        handler.HandleNotificationCall(method, request[KEY_REQUEST_PARAMETERS]);
        response = Json::nullValue;
    }
}

// �����������壺
//      request���ͻ��˳��������˳����͵�json��ʽ��RPC����������飩
// ===============================================================================================================
// ��������
// Validate��ȷ�ϣ�֤ʵ
// ����ͨ���У��ڽ��յ���ȷ��Ϣ����ȷ��Ϣ��������ͷ����͵�һ���ź�
// ����˳���ȷ�ϣ���飩���ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request�У������ĸ����ֶ�ֵ��
// �Ƿ�Ϸ�����������Json-Rpc v2.0Э��Ĺ淶��
// ===============================================================================================================
int AbstractProtocolHandler::ValidateRequest(const Json::Value &request)
{
    int error = 0;
    Procedure proc;
    // Validate��ȷ�ϣ�֤ʵ
    // ����ͨ���У��ڽ��յ���ȷ��Ϣ����ȷ��Ϣ��������ͷ����͵�һ���ź�
    // ����˳���ȷ�ϣ���飩���ͻ��˳��򣬷��͸�����˳����Json��ʽ���������request�У������ĸ����ֶ�ֵ���Ƿ�Ϸ�����������Json-Rpc v2.0Э��Ĺ淶��
    // Json-Rpc V2.0 Э����ͨԶ�̹��̵���ʾ�����£�
    // �ͻ��������˷��͵ģ�json��ʽ��RPC�����������:
    // --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
    // ����˸��ͻ��˷��صģ�json��ʽ����Ӧ�������£�
    // <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
    if (!this->ValidateRequestFields(request))
    {
        error = Errors::ERROR_RPC_INVALID_REQUEST;
    }
    else
    {
        // ����˳����Զ�̹����б�procedures���������й��ͻ��˳�����е��õ�Զ�̹���
        // Զ�̹��̵ĺ��壺
        // ��1��Զ�̷���
        // ��2��Զ��֪ͨ
        // �ڷ���˳����Զ�̹����б�procedures�У����ҿͻ��˳��������������request�е���������Զ�̹��̵�����
        map<string, Procedure>::iterator it =
            this->procedures.find(request[KEY_REQUEST_METHODNAME].asString());
        // �������ݣ����ǶԿͻ��˳��������������request�е����ݣ�������֤
        if (it != this->procedures.end())
        {
            proc = it->second;
            if (this->GetRequestType(request) == RPC_METHOD &&
                    proc.GetProcedureType() == RPC_NOTIFICATION)
            {
                error = Errors::ERROR_SERVER_PROCEDURE_IS_NOTIFICATION;
            }
            else if (this->GetRequestType(request) == RPC_NOTIFICATION &&
                     proc.GetProcedureType() == RPC_METHOD)
            {
                error = Errors::ERROR_SERVER_PROCEDURE_IS_METHOD;
            }
            else if (!proc.ValdiateParameters(request[KEY_REQUEST_PARAMETERS]))
            {
                error = Errors::ERROR_RPC_INVALID_PARAMS;
            }
        }
        else
        {
            error = Errors::ERROR_RPC_METHOD_NOT_FOUND;
        }
    }
    return error;
}
