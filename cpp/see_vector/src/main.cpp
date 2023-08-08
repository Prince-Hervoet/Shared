#include <iostream>
#include "see_vector.hpp"

int main()
{
    letMeSee::SeeVector<int> vector;
    vector.PushBack(45);
    vector.PushBack(434);
    vector.PushBack(41);
    vector.PushBack(23);
    std::cout << vector.Size() << std::endl;
    return 0;
}