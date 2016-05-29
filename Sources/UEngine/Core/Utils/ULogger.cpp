#include "ULogger.h"

ULogger* ULogger::GetInstance()
{
    if (instance == NULL)
    {
        instance = new ULogger();
    }
    return instance;
}

ULogger::ULogger()
{
    //Safe opening
    fopen_s(&fLog, ULOG_FILE_PATH.c_str(), "a");
    Message("ULogger singleton object is created...");
}

ULogger:: ~ULogger()
{
    Message("ULogger singleton object is deleted...");
    fclose(fLog);
}

void ULogger::Message(string text, ULogType msg_type, ULogTarget out)
{
    // Current date and time
    SYSTEMTIME st;
    GetSystemTime(&st);

    static char message[4096];
    string type;

    memset(message, '\0', 4096);

    if (msg_type == ULogType::Info)
        type = "INFO";
    else if (msg_type == ULogType::Error)
        type = "ERROR";
    else if (msg_type == ULogType::Warning)
        type = "WARNING";

    // Formed string (st.wHour +3) = time in Moscow
    //sprintf_s(message,  "[%d/%d/%d %d:%d:%d] %s %s\n", st.wDay,st.wMonth,st.wYear,st.wHour + 3,st.wMinute,st.wSecond,(LPCWSTR)type.c_str(), (LPCWSTR)text.c_str());
    sprintf_s(message, "[%d/%d/%d %d:%d:%d] %s %s\n", st.wDay, st.wMonth, st.wYear, st.wHour + 3, st.wMinute, st.wSecond, (LPCWSTR)type.c_str(), (LPCWSTR)text.c_str());

    if (out == ULogTarget::File)
        fprintf(fLog, message);
    else if (out == ULogTarget::MsgBox)
        MessageBoxA(NULL, text.c_str(), type.c_str(), MB_OK);
}