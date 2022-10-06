#pragma once

#ifndef GENETIC_ALGORITHMS_H_
#define GENETIC_ALGORITHMS_H_

#include "encoding.h"
#include "timer.h"

class GeneticAlgorithm
{
public:

	GeneticAlgorithm(GAEncoding *encoding, int population_size = 1);
	~GeneticAlgorithm();

	void parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement);		// Select Parents
	void recombination();		// Crossover / Mutation to generate offspring
	void survivorSelection();	// Select Survivors

	


private:
	GAEncoding *m_encoding;
	Stopwatch watch;
};


#endif // GENETIC_ALGORITHMS_H_