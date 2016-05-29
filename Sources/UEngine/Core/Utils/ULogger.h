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
    void Message(string text, ULogType msg_type = ULogType::Info, ULogTarget out = ULogTarget::File);
    
    ~ULogger();
};