#include <vector>
#include <iostream>
#include <sstream>
#include <string>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

static inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

static inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

static inline std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

static inline std::vector<std::string> split(std::string str)
{
    std::vector<std::string> res;
    std::stringstream iss(str);

    do
    {
        std::string sub;
        iss >> sub;

        if (iss)
            res.push_back(sub);

    } while (iss);

    return res;
}


