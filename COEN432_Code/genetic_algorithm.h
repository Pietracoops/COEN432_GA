#pragma once

#ifndef GENETIC_ALGORITHMS_H_
#define GENETIC_ALGORITHMS_H_

#include "encoding.h"
#include "timer.h"
#include <filesystem>

class NetworkStatistics
{
public:
	NetworkStatistics();
	std::vector<int> m_max_fitness;
	std::vector<int> m_med_fitness;
	std::vector<int> m_min_fitness;

	int m_stagnation_countdown_val = 10;
	int m_stagnation_modified_countdown;
	bool stagnation_detected;
	bool checkStagnation(int generation_range, int generation, int breath);
	void tuneParameter(float& parameter, const float tune, const float clamp_min, const float clamp_max);
};


class GeneticAlgorithm
{

private:
	struct Parameters { // Have this be initialized using a json file
		// Parent Selection Params
		unsigned int population_size;	// Size of population
		int strategy;					// 0 for fitness proportionate (roulette), 1 for tournament
		uint32_t carry_over;			// Elitism, number of genomes to carry over
		float selection_ratio;			// Number of genomes to select from parents
		uint32_t window_size;			// Specific to tournament selection, number of genomes in local tournament
		bool replacement;				// Specific to tournament selection - if replacement should be used
		float randomness;				// The ratio of randomly initialized new parents that should be added to the pool
		float diversity_ratio;			// threshold of Max_fitness - Min_fitness / Max_fitness
		float purge_ratio;				// If diversity ratio is reached, purge_ratio is ratio of pop to re-initialize

		// Recombination parameters
		float crossoverProb;
		bool skipCrossover;
		int goalOffspringSize;

		// Mutation parameters
		float mutationProb;
		bool accelerated;

		// Survivor selection parameters
		int survivorpolicy;
		int survivorsize;

		// Termination Condition Parameters
		int maxGeneration;
		double maxRuntime;
		int targetFitness;

		// Stats
		bool dynamic_hyper_parameters;	// Dynamically tune Mutation Probability and Crossover Probability
		int stagnation_check;			// Check if we have stagnated
		int stagnation_breath;			// Number of generations to wait before addressing stagnation again

		// Misc
		bool save_population;
		bool save_every_x_generation = true;
		int save_every_x_generation_val = 500;

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