#pragma once

#ifndef GENETIC_ALGORITHMS_H_
#define GENETIC_ALGORITHMS_H_

#include "encoding.h"
#include "timer.h"

class GeneticAlgorithm
{

private:
	struct Parameters { // Have this be initialized using a json file
		int strategy;
		uint32_t carry_over;
		float selection_ratio;
		uint32_t window_size;
		bool replacement;

		// Recombination parameters
		float crossoverProb;
		bool allowFailures;
		int goalOffspringSize;

		// Mutation parameters
		float mutationProb;

		// Termination Condition Parameters
		int maxGeneration;
		double maxRuntime;
		int targetFitness;
	};

	GAEncoding* m_encoding;
	Stopwatch watch;
	Stopwatch timeElapsedTimer;
	int generation;

public:

	GeneticAlgorithm(GAEncoding *encoding, int population_size = 1);
	~GeneticAlgorithm();


	void parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement);		// Select Parents
	void recombination(float crossoverProb, int goalOffspringSize, bool allowfailures = false);		// Crossover 
	void mutation(float mutationProb);											// Mutation
	void survivorSelection();	// Select Survivors

	bool terminationConditions(int currentGen, int maxGeneration = -1, double currRuntime = -1, double maxRuntime = -1, int targetFitness = -1);

	void runGA();

	Parameters params;

	void printParameters();
};

#endif // GENETIC_ALGORITHMS_H_