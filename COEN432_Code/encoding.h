#pragma once


#ifndef GAENCODING_H_
#define GAENCODING_H_

#include "helper_functions.h"

class GAEncoding_Ass1
{
	class Tile
	{
	public:
		std::string top;
		std::string right;
		std::string bottom;
		std::string left;

		Tile(std::string constructor = "0000");
		void rotate(unsigned int rot_value = 1);
		void printTile();

	};


public:
	GAEncoding_Ass1(std::string file_name = "");
	~GAEncoding_Ass1();

	// Utility functions
	std::vector<std::vector<int>> returnGenome();
	std::vector<std::vector<std::vector<int>>> returnRandomizedGenome(const unsigned int number_of_genomes);
	void initializaPopulation(const unsigned int number_of_genomes);
	int fitnessOfGenome(std::vector<std::vector<int>> genome);

	// Core Functions
	void parentSelection();		// Select Parents
	void recombination();		// Crossover / Mutation to generate offspring
	void survivorSelection();	// Select Survivors

private:
	int WIDTH = 8;
	int HEIGHT = 8;

	std::map<int, Tile> m_map_index;
	std::vector<std::vector<int>> m_original_genome;


	std::vector<std::vector<std::vector<int>>> m_population;
	std::vector<int> m_population_fitness;

	std::vector<std::vector<std::vector<int>>> m_offspring;

	void encodeToMap(std::vector<std::vector<std::string>> input);


	std::vector<std::string> void_file{ "2310 2016 6521 1120 4420 5306 2635 1514",
										"0164 5024 0520 1132 2500 1226 1144 3544",
										"1012 2025 4501 2553 6231 2645 1004 4051",
										"3622 4056 6303 3145 5122 4403 4125 6322",
										"1152 2304 1256 2034 1605 3436 1242 4114",
										"4614 2444 6311 1120 1022 1026 3503 1345",
										"2364 6451 4236 2104 4514 3452 2340 4313",
										"6111 6146 1412 3002 2651 4121 4120 1456" };
};


#endif // GAENCODING_H_