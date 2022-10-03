#pragma once

#ifndef GENETIC_ALGORITHMS_H_
#define GENETIC_ALGORITHMS_H_

#include "encoding.h"

class GeneticAlgorithm
{
public:

	GeneticAlgorithm(GAEncoding_Ass1 encoding, int population_size = 1);
	~GeneticAlgorithm();

	void parentSelection();

private:
	GAEncoding_Ass1 m_encoding;

};


#endif // GENETIC_ALGORITHMS_H_