#pragma once

#include <string>
#include <stdio.h>
#include <windows.h>

using namespace std;

#include "enum.h"
#include "..\Basic\USingleton.hpp"

class ULogger : public USingleton<ULogger>
{
protected:

    ULogger();
    FILE* fLog;

public:

    static ULogger* GetInstance();
    static const string ULOG_FILE_PATH;
    void Message(string text, ULOG_MSG_TYPE msg_type = ULOG_MSG_INFO, ULOG_OUT out = ULOG_OUT_FILE);
    
    ~ULogger();
};