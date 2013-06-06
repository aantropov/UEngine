#pragma once

#include <string>
#include <stdio.h>
#include <windows.h>

using namespace std;

#include "enum.h"
#include "..\Basic\USingleton.hpp"

// Logging class
class ULogger : public USingleton<ULogger>{
protected:

    ULogger();
    FILE* fLog;

public:
    // Singleton method
    static ULogger* GetInstance();

    // Path to log file
    static const string ULOG_FILE_PATH;
        
    // Write message
    void Message(string text, ULOG_MSG_TYPE msg_type = ULOG_MSG_INFO, ULOG_OUT out = ULOG_OUT_FILE);
    
    ~ULogger();
};

// Singleton
//ULogger* ULogger::instance = NULL;
