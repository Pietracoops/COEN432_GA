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
#include <set>
#include <map>
#include <algorithm>
#include <random>
#include <regex>

#include "logging.h"
#include "Genome.h"


// Read external files into memory and store in string vector
/// \return: Vector containing file contents as strings
std::vector<std::string> readFileIntoMemory(std::string file_path);

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
std::vector<int> shuffleVector(std::vector<int> vect);
std::vector<Genome> shuffleVector(std::vector<Genome> vect);

// Given a string, will return a vector containing all parts of string
// split by given delimiter
/// \return: strings split with delimiter
std::vector<std::string> splitString(std::string str, std::string delimiter);

// Prints a two dimensional vector
/// \return: None
void printVector(std::vector<std::vector<int>> vect);


/**
* Generate a vector of random floats between 0 and 1
*/
std::vector<float> generateRandVecFloat(int n_elem, std::mt19937& engine);

//template<typename T>
//inline std::vector<T> erase_indices(const std::vector<T>& data, std::vector<size_t>& indicesToDelete);
template<typename T>
inline std::vector<T> erase_indices(const std::vector<T>& data, std::vector<size_t>& indicesToDelete/* can't assume copy elision, don't pass-by-value */)
{
	if (indicesToDelete.empty())
		return data;

	std::vector<T> ret;
	ret.reserve(data.size() - indicesToDelete.size());

	std::sort(indicesToDelete.begin(), indicesToDelete.end());

	// new we can assume there is at least 1 element to delete. copy blocks at a time.
	auto itBlockBegin = data.begin();

	for (std::vector<size_t>::const_iterator it = indicesToDelete.begin(); it != indicesToDelete.end(); ++it)
	{
		auto itBlockEnd = data.begin() + *it;
		if (itBlockBegin != itBlockEnd)
		{
			std::copy(itBlockBegin, itBlockEnd, std::back_inserter(ret));
		}
		itBlockBegin = itBlockEnd + 1;
	}

	// copy last block.
	if (itBlockBegin != data.end())
	{
		std::copy(itBlockBegin, data.end(), std::back_inserter(ret));
	}


	return ret;
}


#endif // HELPER_FUNCTIONS_H_