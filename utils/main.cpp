#include <iostream>
#include "logging.h"


int main()
{
    CLogger::Instance().AddAdapter(new CFileAdpter("appname", "E:\\logs"));
    CLogger::Instance().StartLogger(LogLevel::LOG_DEBUG, true);

    while (true) {
        WriteLogA(LOG_INFO, "this is a test");
        Sleep(1000);
    }
    std::cin.get();
    return 0;
}
