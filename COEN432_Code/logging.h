#pragma once

#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <iostream>

inline std::ostream& genetic_algo_log()
{
    return std::cout << "[Genetic Algo  ] ";
}

inline std::ostream& encoding_log()
{
    return std::cout << "[Encoding  ] ";
}


#endif