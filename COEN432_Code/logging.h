#pragma once

#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <iostream>
#include <string>

static std::string LOG_PATH;

inline std::ostream& genetic_algo_log()
{
    return std::cout << "[Genetic Algo  ] ";
}

inline std::ostream& encoding_log()
{
    return std::cout << "[Encoding  ] ";
}

inline std::string getCurrentDateTime(std::string s)
{
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    localtime_s(&tstruct, &now);
    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d-%X;", &tstruct);
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", &tstruct);
    return std::string(buf);
};

inline void Logger(std::string logMsg) 
{
    if (!LOG_PATH.empty())
    {
        std::string now = getCurrentDateTime("now");
        std::ofstream ofs(LOG_PATH.c_str(), std::ios_base::out | std::ios_base::app);
        ofs << now << '\t' << logMsg << '\n';
        ofs.close();
    }

}

#endif