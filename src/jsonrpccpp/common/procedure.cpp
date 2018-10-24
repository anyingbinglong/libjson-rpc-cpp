/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    procedure.cpp
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include "procedure.h"
#include "errors.h"
#include "exception.h"
#include <cstdarg>
#include <vector>

using namespace std;
using namespace jsonrpc;

Procedure::Procedure()
    : procedureName(""), procedureType(RPC_METHOD), returntype(JSON_BOOLEAN),
      paramDeclaration(PARAMS_BY_NAME) {}

Procedure::Procedure(const string &name, parameterDeclaration_t paramType,
                     jsontype_t returntype, ...)
{
    // 定义一个字符指针变量，即：char* parameters，
    // 可以理解为指向可变参数列表“...”中当前参数的一个指针，获取可参数列表“...”中的参数必须通过这个指针进行
    va_list parameters;
    // 可变参数列表“...”中，第二个参数的值
    va_start(parameters, returntype);

    // 获取可变参数列表“...”中，第一个参数的值
    // 同时，将parameters指向可变参数列表“...”中，第二个参数
    const char *paramname = va_arg(parameters, const char *);
    jsontype_t type;
    while (paramname != NULL)
    {
        // 第一次进入while循环时，执行流程如下：
        // 获取可变参数列表“...”中，第二个参数的值
        // 同时，将parameters指向可变参数列表“...”中，第三个参数
        type = (jsontype_t)va_arg(parameters, int);
        // 将可变参数列表“...”中，第一个参数的值，和，可变参数列表“...”中，第二个参数的值，保存起来，放到相应的成员变量中
        this->AddParameter(paramname, type);
        // 获取可变参数列表“...”中，第三个参数的值
        // 同时，将parameters指向可变参数列表“...”中，第四个参数
        paramname = va_arg(parameters, const char *);
    }
    va_end(parameters);
    this->procedureName = name;
    this->returntype = returntype;
    this->procedureType = RPC_METHOD;
    this->paramDeclaration = paramType;
}
Procedure::Procedure(const string &name, parameterDeclaration_t paramType, ...)
{
    // 定义一个字符指针变量，即：char* parameters，
    // 可以理解为指向可变参数列表“...”中当前参数的一个指针，获取可参数列表“...”中的参数必须通过这个指针进行
    va_list parameters;

     // 可变参数列表“...”中，第二个参数的值
    va_start(parameters, paramType);

    // 获取可变参数列表“...”中，第一个参数的值
    // 同时，将parameters指向可变参数列表“...”中，第二个参数
    const char *paramname = va_arg(parameters, const char *);
    jsontype_t type;
    while (paramname != NULL)
    {
        // 第一次进入while循环时，执行流程如下：
        // 获取可变参数列表“...”中，第二个参数的值
        // 同时，将parameters指向可变参数列表“...”中，第三个参数
        type = (jsontype_t)va_arg(parameters, int);
        // 将可变参数列表“...”中，第一个参数的值，和，可变参数列表“...”中，第二个参数的值，保存起来，放到相应的成员变量中
        this->AddParameter(paramname, type);
        // 获取可变参数列表“...”中，第三个参数的值
        // 同时，将parameters指向可变参数列表“...”中，第四个参数
        paramname = va_arg(parameters, const char *);
    }
    va_end(parameters);
    this->procedureName = name;
    this->procedureType = RPC_NOTIFICATION;
    this->paramDeclaration = paramType;
    this->returntype = JSON_BOOLEAN;
}

bool Procedure::ValdiateParameters(const Json::Value &parameters) const
{
    if (this->parametersName.empty())
    {
        return true;
    }
    if (parameters.isArray() && this->paramDeclaration == PARAMS_BY_POSITION)
    {
        return this->ValidatePositionalParameters(parameters);
    }
    else if (parameters.isObject() &&
             this->paramDeclaration == PARAMS_BY_NAME)
    {
        return this->ValidateNamedParameters(parameters);
    }
    else
    {
        return false;
    }
}
const parameterNameList_t &Procedure::GetParameters() const
{
    return this->parametersName;
}
procedure_t Procedure::GetProcedureType() const
{
    return this->procedureType;
}
const std::string &Procedure::GetProcedureName() const
{
    return this->procedureName;
}
parameterDeclaration_t Procedure::GetParameterDeclarationType() const
{
    return this->paramDeclaration;
}
jsontype_t Procedure::GetReturnType() const
{
    return this->returntype;
}

void Procedure::SetProcedureName(const string &name)
{
    this->procedureName = name;
}
void Procedure::SetProcedureType(procedure_t type)
{
    this->procedureType = type;
}
void Procedure::SetReturnType(jsontype_t type)
{
    this->returntype = type;
}
void Procedure::SetParameterDeclarationType(parameterDeclaration_t type)
{
    this->paramDeclaration = type;
}

void Procedure::AddParameter(const string &name, jsontype_t type)
{
    this->parametersName[name] = type;
    this->parametersPosition.push_back(type);
}
bool Procedure::ValidateNamedParameters(const Json::Value &parameters) const
{
    bool ok = parameters.isObject() || parameters.isNull();
    for (map<string, jsontype_t>::const_iterator it =
                this->parametersName.begin();
            ok == true && it != this->parametersName.end(); ++it)
    {
        if (!parameters.isMember(it->first))
        {
            ok = false;
        }
        else
        {
            ok = this->ValidateSingleParameter(it->second, parameters[it->first]);
        }
    }
    return ok;
}
bool Procedure::ValidatePositionalParameters(
    const Json::Value &parameters) const
{
    bool ok = true;

    if (parameters.size() != this->parametersPosition.size())
    {
        return false;
    }

    for (unsigned int i = 0; ok && i < this->parametersPosition.size(); i++)
    {
        ok = this->ValidateSingleParameter(this->parametersPosition.at(i),
                                           parameters[i]);
    }
    return ok;
}
bool Procedure::ValidateSingleParameter(jsontype_t expectedType,
                                        const Json::Value &value) const
{
    bool ok = true;
    switch (expectedType)
    {
    case JSON_STRING:
        if (!value.isString())
            ok = false;
        break;
    case JSON_BOOLEAN:
        if (!value.isBool())
            ok = false;
        break;
    case JSON_INTEGER:
        if (!value.isIntegral())
            ok = false;
        break;
    case JSON_REAL:
        if (!value.isDouble())
            ok = false;
        break;
    case JSON_OBJECT:
        if (!value.isObject())
            ok = false;
        break;
    case JSON_ARRAY:
        if (!value.isArray())
            ok = false;
        break;
    }
    return ok;
}
