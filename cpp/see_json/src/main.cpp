#include "see_json.hpp"
#include <iostream>

int main()
{
    letMeSee::SeeJson json;
    letMeSee::SeeJson jsonArr1;
    json.Set("123123", "19889");
    json.Set("11", true);
    jsonArr1.append(123);
    jsonArr1.append("123123213123123");
    jsonArr1.append(false);
    jsonArr1.append(json);
    std::cout << jsonArr1.Stringify() << std::endl;
    jsonArr1.freeJson();
    // json.freeJson();
    return 0;
}
