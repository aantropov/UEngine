#include "utils.h"
#include <iostream>
#include <sstream>
#include <string>

std::vector<std::string> Split(std::string str)
{
    std::vector<std::string> res;
    std::stringstream iss(str);

    do
    {
        std::string sub;
        iss >> sub;
        res.push_back(sub);
    } while (iss);
        
    return res;
}
