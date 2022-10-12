#pragma once


#ifndef GAENCODING_H_
#define GAENCODING_H_


#include "helper_functions.h"


class GAEncoding
{
public:

	virtual void initializaPopulation(const unsigned int number_of_genomes) = 0;

	// Core Functions
	virtual void survivorSelection(int policy = 0, int survivorSize = 0) = 0;		// Select Survivors from the parents + offspring
	virtual void recombination(float crossoverProb, int goalOffspringSize, bool skipCrossover) = 0;		// Crossover to generate offspring
	virtual void mutation(float mutationProb, bool accelerated) = 0;

	// Parent Selection
	virtual void parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement, float randomness, float diversity_ratio, float purge_ratio) = 0;
	virtual std::vector<Genome> parentSelectionFitnessProportionate(std::vector<Genome> population, float selection_ratio) = 0;
	virtual std::vector<Genome> parentSelectionTournament(std::vector<Genome> population, float selection_ratio, uint32_t window_size, bool replacement) = 0;

	// Mutation Functions
	virtual void permutationRandomSwap(Genome& gen, const uint32_t num_of_swaps) = 0;
	virtual void permutationSwap(Genome& gen, const uint32_t pos1, const uint32_t pos2) = 0;
	virtual void permutationInsert(Genome& gen, const uint32_t initial_pos, const uint32_t final_pos) = 0;
	virtual void permutationRandomInsert(Genome& gen) = 0;
	virtual void permutationScramble(Genome& gen, std::vector<int> indices) = 0;
	virtual void permutationRandomScramble(Genome& gen, float mutation_ratio) = 0;
	virtual void permutationInvert(Genome& gen, std::vector<int> indices) = 0;
	virtual void permutationRandomInvert(Genome& gen, float mutation_ratio) = 0;
	virtual void permutationPointMutation(Genome& gen, unsigned int pos, unsigned int rot) = 0;
	virtual void permutationRandomPointMutation(Genome& gen, float mutation_ratio) = 0;
	virtual void permutationRandomDiversify(std::vector<Genome>& gen_v, const float purge_ratio) = 0;
	virtual void permutationSlide(Genome& gen) = 0;
	virtual void injectParents(float proportion) = 0;
	virtual void savePopulation() = 0;
	virtual void loadPopulation(std::string file_name, unsigned int starting_pop_size) = 0;

	// Utility Functions
	virtual Genome getGenomeFromPopulation(const unsigned int gen_num) = 0;
	virtual float getAverageFitness(std::vector<Genome> population) = 0;

	// Termination Condition Functions
	virtual bool terminationConditions(int currentGen, int maxGeneration, double currRuntime, double maxRuntime, int targetFitness) = 0;

	// Population that is generated and manipulated
	std::vector<Genome> m_population;

	// The selected parents from the general population
	std::vector<Genome> m_parents;

	// Offspring that are generated from their parents
	std::vector<Genome> m_offspring;

	// Offspring that are generated from their parents
	std::vector<Genome> m_elite;

	// Parameters
	int m_max_fitness;
	int m_med_fitness;
	int m_min_fitness;

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
	Genome returnRandomlyInitializedGenome();
	virtual void initializaPopulation(const unsigned int number_of_genomes) override;
	std::vector<int> fitnessOfGenome(const Genome& genome);

	// Core Functions
	
	virtual void recombination(float crossoverProb, int goalOffspringSize, bool skipCrossover = false) override;	// Crossover 
	virtual void mutation(float mutationProb, bool accelerated) override;
	virtual void survivorSelection(int policy = 0, int survivorSize = 0) override;							// Select Survivors

	// Parent Selection
	virtual void parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement, float randomness, float diversity_ratio, float purge_ratio);
	virtual std::vector<Genome> parentSelectionFitnessProportionate(std::vector<Genome> population, float selection_ratio) override;
	virtual std::vector<Genome> parentSelectionTournament(std::vector<Genome> population, float selection_ratio, uint32_t window_size, bool replacement) override;

	// Survivor selection policies
	std::vector<Genome> uFromGammaPolicy(int survivorSize = 0);
	std::vector<Genome> uPlusGammaPolicy(int survivorSize = 0);
	std::vector<Genome> uFromGammaPolicy_FUDS(int survivorSize = 0); // Policy 2


	// Mutation Functions
	void permutationRandomSwap(Genome& gen, const uint32_t num_of_swaps) override;
	void permutationSwap(Genome& gen, const uint32_t pos1, const uint32_t pos2) override;
	void permutationInsert(Genome& gen, const uint32_t initial_pos, const uint32_t final_pos) override;
	void permutationRandomInsert(Genome& gen) override;
	void permutationScramble(Genome& gen, std::vector<int> indices) override;
	void permutationRandomScramble(Genome& gen, float mutation_ratio) override;
	void permutationInvert(Genome& gen, std::vector<int> indices) override;
	void permutationRandomInvert(Genome& gen, float mutation_ratio) override;
	void permutationPointMutation(Genome& gen, unsigned int pos, unsigned int rot) override;
	void permutationRandomPointMutation(Genome& gen, float mutation_ratio) override;
	void permutationRandomDiversify(std::vector<Genome>& gen_v, const float purge_ratio) override;
	void permutationSlide(Genome& gen) override;
	void permutationRandomScrambleOld(Genome& gen);
	void permutationRandomInvertOld(Genome& gen);
	void permutationRandomPointMutationOld(Genome& gen);


	// Crossover Functions
	std::vector<Genome> singlePointCrossover(Genome& parent1, Genome& parent2);
	std::vector<Genome> partiallyMappedCrossover(Genome& parent1, Genome& parent2, std::vector<unsigned int> cross_indices);

	// Termination Condition functions
	virtual bool terminationConditions(int currentGen, int maxGeneration, double currRuntime, double maxRuntime, int targetFitness) override;

	// Utility Functions
	virtual Genome getGenomeFromPopulation(const unsigned int gen_num) override;
	virtual float getAverageFitness(std::vector<Genome> population) override;
	virtual void injectParents(float proportion) override;

	void savePopulation() override;
	void loadPopulation(std::string file_name, unsigned int starting_pop_size) override;

	Genome getEliteFromFile(std::string file_name);
	std::string genotypeToPhenotype(Genome g);


	// The original genome that was extracted from the input file
	std::vector<std::vector<int>> m_original_genome;

private:

	// Random tools
	std::random_device rd;
	std::mt19937::result_type seed;
	std::mt19937 gen_mt;

	int WIDTH = 8;
	int HEIGHT = 8;

	int starting_pop_size;

	uint32_t MAX_MISMATCHES = 112;


	// Map containing the indexes of each tile (1 - 64) and the corresponding Tile
	// Tile object where each individual tile of the puzzle can be stored
	std::map<int, Tile> m_map_index;

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