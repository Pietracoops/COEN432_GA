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

	void parentSelection();		// Select Parents
	void recombination(float crossoverProb);		// Crossover 
	void survivorSelection();	// Select Survivors


private:
	GAEncoding *m_encoding;
	Stopwatch watch;
};


#endif // GENETIC_ALGORITHMS_H_