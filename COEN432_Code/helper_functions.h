#pragma once

#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_

// Filesystem utilities
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <vector>
#include <array>
#include <set>
#include <map>
#include <algorithm>
#include <random>
#include <regex>
#include <functional>

#include "logging.h"
#include "Genome.h"


// Read external files into memory and store in string vector
/// \return: Vector containing file contents as strings
std::vector<std::string> readFileIntoMemory(std::string file_path);

// Shuffles a two dimensional vector - note: this function should be overloaded
// Overloaded variants listed as well
/// \return: Shuffled input vector
std::vector<std::vector<int>> shuffleVector(std::vector<std::vector<int>> vect);
std::vector<int> shuffleVector(std::vector<int> vect);
std::vector<Genome> shuffleVector(std::vector<Genome> vect);

// Saves a phenotype to a file
/// \return: void - file output
void savePhenotypeToFile(std::string p, std::string filename);

// Given a string, will return a vector containing all parts of string
// split by given delimiter
/// \return: strings split with delimiter
std::vector<std::string> splitString(std::string str, std::string delimiter);

// Generates the indices for a random bounding box in a 2D vector
std::vector<unsigned int> getBoundingBox(int col_dim, int row_dim, std::mt19937& engine, int index1 = -1, int index2 = -1, int max_area = -1, int max_cols = -1, int max_rows = -1);

// Converts a 1 Dimensional vector to a 2 Dimensional vector
/// \return: vector of ints (2D)
std::vector<std::vector<int>> vectTo2D(std::vector<int> vect_input, uint32_t row, uint32_t col);

// Slides a 2 Dimensional box within a larger bounding box and displaces
// All other boxes accordingly
/// \return: New 2 Dimensional bounding box with displaced indices
std::vector<std::vector<int>> vectSlide(std::vector<std::vector<int>>vect_input, std::vector<std::vector<int>> indices, int slide, bool vertical);

// Prints a two dimensional vector
/// \return: None
void printVector(std::vector<std::vector<int>> vect);

// Generates a vector of random floats
/// \return: New Vector containing random floats
std::vector<float> generateRandVecFloat(int n_elem, std::mt19937& engine);

// Erase indices from a vector
// Template used to be able to use any type of vector
/// \return: New Vector containing altered indices
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

// PROFESSOR FUNCTION: Perform the rows and columns mismatch checking that was provided with assignment 1
/// \return: Number of mismatches in genome
int ProfTest(std::string file_path);

// PROFESSOR FUNCTION: Used by ProfTest to count the number of mismatches in columns of box puzzle
/// \return: Number of mismatches between columns of two designated rows
int CountColumnMismatches(std::vector<std::string> first, std::vector<std::string> second);

// PROFESSOR FUNCTION: Used by ProfTest to count the number of mismatches in rows of box puzzle
/// \return: Number of mismatches between rows of two designated columns
int CountRowMismatches(std::vector<std::string> first, std::vector<std::string> second);

#endif // HELPER_FUNCTIONS_H_