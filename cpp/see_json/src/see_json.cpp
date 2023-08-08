#include "see_json.hpp"
#include <iostream>

namespace letMeSee
{
    SeeJson::SeeJson()
        : type(JSON_NULL)
    {
        data.valueDouble = 0.0;
    }

    SeeJson::SeeJson(bool value)
        : type(JSON_BOOL)
    {
        data.valueBool = value;
    }

    SeeJson::SeeJson(int value)
        : type(JSON_INT)
    {
        data.valueInt = value;
    }

    SeeJson::SeeJson(std::string &value)
        : type(JSON_STRING)
    {
        data.valueString = &value;
    }

    SeeJson::SeeJson(const char *value)
        : type(JSON_STRING)
    {
        data.valueString = new std::string(value);
    }

    SeeJson::~SeeJson()
    {
    }

    SeeJson &SeeJson::operator[](const char *key)
    {
        if (key == nullptr)
        {
            throw "error: The key is null";
        }
        if (type != JSON_OBJECT)
        {
            throw "error: type mismatch";
        }

        if (!data.valueObject)
        {
            data.valueObject = new std::map<std::string, SeeJson>();
        }
        auto obj = *(data.valueObject);
        if (obj.count(key) == 0)
        {
            obj[key] = SeeJson();
        }
        return obj[key];
    }

    void SeeJson::Set(std::string &key, const SeeJson &other)
    {
        if (type != JSON_NULL && type != JSON_OBJECT)
            return;
        type = JSON_OBJECT;
        init();
        data.valueObject->insert(std::pair<std::string, SeeJson>(key, other));
    }

    void SeeJson::Set(const char *key, const SeeJson &other)
    {
        if (type != JSON_NULL && type != JSON_OBJECT)
            return;
        type = JSON_OBJECT;
        init();
        data.valueObject->insert(std::pair<std::string, SeeJson>(key, other));
    }

    void SeeJson::operator=(bool value)
    {
        if (type != JSON_NULL && type != JSON_BOOL)
            return;
        data.valueBool = value;
        type = JSON_BOOL;
    }

    void SeeJson::operator=(int value)
    {
        if (type != JSON_NULL && type != JSON_INT)
            return;
        data.valueInt = value;
        type = JSON_INT;
    }

    void SeeJson::operator=(double value)
    {
        if (type != JSON_NULL && type != JSON_DOUBLE)
            return;
        data.valueDouble = value;
        type = JSON_DOUBLE;
    }

    void SeeJson::operator=(std::string &value)
    {
        if (type != JSON_NULL && type != JSON_STRING)
            return;
        data.valueString = &value;
        type = JSON_STRING;
    }

    void SeeJson::operator=(const char *value)
    {
        if (type != JSON_NULL && type != JSON_STRING)
            return;
        data.valueString = new std::string(value);
        type = JSON_STRING;
    }

    void SeeJson::init()
    {
        if (type == JSON_OBJECT && data.valueObject == nullptr)
        {
            data.valueObject = new std::map<std::string, SeeJson>();
        }
        else if (type == JSON_ARRAY && data.valueArray == nullptr)
        {
            data.valueArray = new std::vector<SeeJson>();
            data.valueArray->reserve(8);
        }
    }

    std::string SeeJson::Stringify()
    {
        std::stringstream ans;
        switch (type)
        {
        case JSON_NULL:
            ans << "null";
            break;
        case JSON_BOOL:
            ans << (data.valueBool ? "true" : "false");
            break;
        case JSON_INT:
            ans << data.valueInt;
            break;
        case JSON_DOUBLE:
            ans << data.valueDouble;
            break;
        case JSON_STRING:
            ans << '\"' << *(data.valueString) << '\"';
            break;
        case JSON_OBJECT:
            ans << '{';
            for (auto it = data.valueObject->begin(); it != data.valueObject->end(); it++)
            {
                if (it != data.valueObject->begin())
                    ans << ',';
                ans << '\"' << it->first << '\"' << ':' << it->second.Stringify();
            }
            ans << '}';
            break;
        case JSON_ARRAY:
            ans << '[';
            for (int i = 0; i < data.valueArray->size(); i++)
            {
                ans << (data.valueArray)->at(i).Stringify();
                if (i < data.valueArray->size() - 1)
                    ans << ',';
            }
            ans << ']';
            break;
        default:
            break;
        }
        return ans.str();
    }

    void SeeJson::append(const SeeJson &other)
    {
        if (type != JSON_NULL && type != JSON_ARRAY)
            return;
        type = JSON_ARRAY;
        init();
        data.valueArray->push_back(other);
    }

    void SeeJson::freeJson()
    {
        if (type == JSON_NULL)
            return;
        switch (type)
        {
        case JSON_STRING:
            delete data.valueString;
            break;
        case JSON_ARRAY:
            for (int i = 0; i < data.valueArray->size(); i++)
            {
                (data.valueArray)->at(i).freeJson();
            }
            delete data.valueArray;
            break;
        case JSON_OBJECT:
            for (auto it = data.valueObject->begin(); it != data.valueObject->end(); it++)
            {
                it->second.freeJson();
            }
            delete data.valueObject;
        default:
            break;
        }
        data.valueDouble = 0.0;
        type = JSON_NULL;
    }
}