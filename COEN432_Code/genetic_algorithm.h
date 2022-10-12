#pragma once

#ifndef GENETIC_ALGORITHMS_H_
#define GENETIC_ALGORITHMS_H_

#include "encoding.h"
#include "timer.h"
#include <filesystem>

class GeneticAlgorithm
{

private:
	struct Parameters { // Have this be initialized using a config file if time permits

		// Parent Selection Params
		unsigned int population_size;			// Size of population
		int strategy;							// 0 for fitness proportionate (roulette), 1 for tournament
		uint32_t carry_over;					// Elitism, number of genomes to carry over
		float selection_ratio;					// Number of genomes to select from parents
		uint32_t window_size;					// Specific to tournament selection, number of genomes in local tournament
		bool replacement;						// Specific to tournament selection - if replacement should be used
		float randomness;						// The ratio of randomly initialized new parents that should be added to the pool
		float diversity_ratio;					// threshold of Max_fitness - Min_fitness / Max_fitness
		float purge_ratio;						// If diversity ratio is reached, purge_ratio is ratio of pop to re-initialize

		// Recombination parameters
		float crossoverProb;					// Probability of a parent to be selected for crossover reproduction
		bool skipCrossover;						// Skip crossover step
		int goalOffspringSize;					// Desired size of offspring pool

		// Mutation parameters
		float mutationProb;						// Probability of a gene in a genome to get mutated
		bool accelerated;						// Accelerated mutation - random mutation sizes applied to genomes (accelerated climb rate early on)

		// Survivor selection parameters
		int survivorpolicy;						// 0 is ufromgamma, 1 is uplusgamma, 2 is uplusgamma fuds
		int survivorsize;						// Desired size of population after the generation loop (generally population size)

		// Termination Condition Parameters
		int maxGeneration;						// Max number of generation loops to perform
		double maxRuntime;						// Max run time in seconds
		int targetFitness;						// Target fitness for termination condition

		// Stagnation mitigation parameters
		bool inject_parents;					// Introduce random parents into the population
		float random_parent_proportion;			// Ratio of new parents to be introduce (with respect to the current parent size)


		// Stats
		bool dynamic_hyper_parameters;			// Dynamically tune Mutation Probability and Crossover Probability
		int stagnation_check;					// The number of generations to wait before flagging a stagnation

		// Misc
		bool save_population;					// Flag to save the population
		bool save_every_x_generation = true;	// Flag to save population every number of generations
		int save_every_x_generation_val = 500;	// Specify number of generations to save the population


	};

	GAEncoding* m_encoding;
	Stopwatch m_watch;
	Stopwatch m_time_elapsed_timer;
	int m_generation;

	std::string m_log_path;

	
	NetworkStatistics stats; // Init Statistics Object

public:

	GeneticAlgorithm(GAEncoding *encoding, int population_size = 1, std::string log_path = "");
	~GeneticAlgorithm();

	void initializePopulation(int population_size);																				// Initialize Pop
	void parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement,
						float randomness, float diversity_ratio, float purge_ratio);											// Select Parents
	void recombination(float crossoverProb, int goalOffspringSize, bool skipCrossover = false);									// Crossover 
	void mutation(float mutationProb, bool accelerated);																							// Mutation
	void survivorSelection(int policy, int survivorSize);																									// Select Survivors

	bool terminationConditions(int currentGen, int maxGeneration = -1, double currRuntime = -1, double maxRuntime = -1, int targetFitness = -1);

	void runGA(std::string population_file = "");
	std::string printParameters();

	

	Parameters m_params;

	
};

#endif // GENETIC_ALGORITHMS_H_