#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <memory>

namespace letMeSee
{
    enum JsonType
    {
        JSON_NULL,
        JSON_BOOL,
        JSON_INT,
        JSON_DOUBLE,
        JSON_OBJECT,
        JSON_ARRAY,
        JSON_STRING
    };

    typedef enum JsonType JsonType;

    class SeeJson
    {
    public:
        SeeJson();
        ~SeeJson();

        SeeJson &operator[](const char *key);

        void Set(std::string &key, const SeeJson &other);
        void Set(const char *key, const SeeJson &other);
        void append(const SeeJson &other);

        void operator=(bool value);
        void operator=(int value);
        void operator=(double value);
        void operator=(std::string &value);
        void operator=(const char *value);

        SeeJson(bool value);
        SeeJson(int value);
        SeeJson(double value);
        SeeJson(std::string &value);
        SeeJson(const char *value);
        // void operator=(const SeeJson &other);

        std::string Stringify();
        void freeJson();

    private:
        void init();

        typedef union
        {
            bool valueBool;
            int valueInt;
            double valueDouble;
            std::string *valueString;
            std::vector<SeeJson> *valueArray;
            std::map<std::string, SeeJson> *valueObject;
        } Data;

        Data data;
        JsonType type;
    };
}