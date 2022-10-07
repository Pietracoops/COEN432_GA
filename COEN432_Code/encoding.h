#pragma once


#ifndef GAENCODING_H_
#define GAENCODING_H_

#include "helper_functions.h"

class Genome
{
private:
	int fitness = 0;
public:
	Genome() {};
	inline Genome(std::vector<std::vector<int>> vec) { this->genome_encoding_2b2_int = vec; }
	std::vector<std::vector<int>> genome_encoding_2b2_int;
	size_t getSize() const;
	void setFitness(int f) { fitness = f; }
	int getFitness() const { return fitness; }
	std::vector<std::vector<int>> getEncoding() const { return genome_encoding_2b2_int; }
	
};


class GAEncoding
{
public:

	virtual void initializaPopulation(const unsigned int number_of_genomes) = 0;

	// Core Functions
	virtual void survivorSelection() = 0;	// Select Survivors from the parents + offspring
	virtual void recombination(float crossoverProb) = 0;		// Crossover to generate offspring

	// Parent Selection
	virtual void parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement) = 0;
	virtual std::vector<Genome> parentSelectionFitnessProportionate(std::vector<Genome> population, float selection_ratio) = 0;
	virtual std::vector<Genome> parentSelectionTournament(std::vector<Genome> population, float selection_ratio, uint32_t window_size, bool replacement) = 0;

	// Mutation Functions
	virtual void permutationRandomSwap(Genome& gen, const uint32_t num_of_swaps) = 0;
	virtual void permutationSwap(Genome& gen, const uint32_t pos1, const uint32_t pos2) = 0;
	virtual void permutationInsert(Genome& gen, const uint32_t initial_pos, const uint32_t final_pos) = 0;
	virtual void permutationScramble(Genome& gen, std::vector<int> indices) = 0;
	virtual void permutationInvert(Genome& gen, std::vector<int> indices) = 0;
	virtual void permutationPointMutation(Genome& gen, unsigned int pos) = 0;


	// Utility Functions
	virtual Genome getGenomeFromPopulation(const unsigned int gen_num) = 0;


	// Population that is generated and manipulated
	std::vector<Genome> m_population;

	// The selected parents from the general population
	std::vector<Genome> m_parents;

	// Offspring that are generated from their parents
	std::vector<Genome> m_offspring;

	// TODO: Create a champions vector that keeps the top n or x% highest performing parents

};

class GAEncoding_Ass1 : public GAEncoding
{
	// Tile Class containing any operations required to manipulate Tiles
	class Tile
	{
	public:
		std::string top;
		std::string right;
		std::string bottom;
		std::string left;

		Tile(std::string constructor = "0000");
		void rotate(unsigned int rot_value = 1); // Rotate the tile up to 3 times
		void printTile();					     // Print tile information

	};

public:
	GAEncoding_Ass1(std::string file_name = "");
	~GAEncoding_Ass1();

	// Utility functions
	std::vector<std::vector<int>> returnGenome();
	std::vector<std::vector<std::vector<int>>> returnRandomizedGenome(const unsigned int number_of_genomes);
	virtual void initializaPopulation(const unsigned int number_of_genomes) override;
	int fitnessOfGenome(const Genome& genome);

	// Core Functions
	
	virtual void recombination(float crossoverProb) override;			// Crossover 
	virtual void survivorSelection() override;		// Select Survivors

	// Parent Selection
	virtual void parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement);
	virtual std::vector<Genome> parentSelectionFitnessProportionate(std::vector<Genome> population, float selection_ratio) override;
	virtual std::vector<Genome> parentSelectionTournament(std::vector<Genome> population, float selection_ratio, uint32_t window_size, bool replacement) override;

	// Mutation Functions
	virtual void permutationRandomSwap(Genome& gen, const uint32_t num_of_swaps) override;
	virtual void permutationSwap(Genome& gen, const uint32_t pos1, const uint32_t pos2) override;
	virtual void permutationInsert(Genome& gen, const uint32_t initial_pos, const uint32_t final_pos) override;
	virtual void permutationScramble(Genome& gen, std::vector<int> indices) override;
	virtual void permutationInvert(Genome& gen, std::vector<int> indices) override;
	virtual void permutationPointMutation(Genome& gen, unsigned int pos) override;

	// Crossover Functions
	std::vector<Genome> singlePointCrossover(Genome& parent1, Genome& parent2);

	// Utility Functions
	virtual Genome getGenomeFromPopulation(const unsigned int gen_num);

private:

	// Random tools
	std::random_device rd;
	std::mt19937::result_type seed;
	std::mt19937 gen_mt;

	int WIDTH = 8;
	int HEIGHT = 8;
	uint32_t MAX_MISMATCHES = 112;

	// Map containing the indexes of each tile (1 - 64) and the corresponding Tile
	// Tile object where each individual tile of the puzzle can be stored
	std::map<int, Tile> m_map_index;

	// The original genome that was extracted from the input file
	std::vector<std::vector<int>> m_original_genome;

	// Encode the input from file to the m_map_index variable
	void encodeToMap(std::vector<std::vector<std::string>> input);

	// Void file in case input file cannot be found
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