#pragma once

#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_

// Filesystem utilities
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

// XTensor Utilities
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xview.hpp>


#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <chrono>
#include "logging.h"

// Read external files into memory and store in string vector
/// \return: Vector containing file contents as strings
std::vector<std::string> readFileIntoMemory(std::string file_path, std::ostream ostream);

// Perform the rows and columns mismatch checking that was provided with assignment 1
/// \return: Number of mismatches in genome
int ProfTest(std::string file_path);

// Used by ProfTest to count the number of mismatches in columns of box puzzle
/// \return: Number of mismatches between columns of two designated rows
int CountColumnMismatches(std::vector<std::string> first, std::vector<std::string> second);

// Used by ProfTest to count the number of mismatches in rows of box puzzle
/// \return: Number of mismatches between rows of two designated columns
int CountRowMismatches(std::vector<std::string> first, std::vector<std::string> second);

// Shuffles a two dimensional vector - note: this function should be overloaded
/// \return: Shuffled input vector
std::vector<std::vector<int>> shuffleVector(std::vector<std::vector<int>> vect);

// Prints a two dimensional vector
/// \return: None
void printVector(std::vector<std::vector<int>> vect);

#endif // HELPER_FUNCTIONS_H_