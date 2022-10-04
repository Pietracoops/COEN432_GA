#pragma once

#ifndef GENETIC_ALGORITHMS_H_
#define GENETIC_ALGORITHMS_H_

#include "encoding.h"
#include "timer.h"

class GeneticAlgorithm
{
public:

	GeneticAlgorithm(GAEncoding_Ass1 encoding, int population_size = 1);
	~GeneticAlgorithm();

	void parentSelection();		// Select Parents
	void recombination();		// Crossover / Mutation to generate offspring
	void survivorSelection();	// Select Survivors


private:
	GAEncoding_Ass1 m_encoding;
	Stopwatch watch;
};


#endif // GENETIC_ALGORITHMS_H_