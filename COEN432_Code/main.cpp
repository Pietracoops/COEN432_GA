#include <iostream>
#include "helper_functions.h"
#include "encoding.h"
#include "genetic_algorithm.h"

int main()
{
	// ############### Initialize the Encoding based on the input file
	GAEncoding_Ass1 Encoding_Ass1("Ass1/Ass1Input.txt");
	GAEncoding* Encoding = &Encoding_Ass1;

	//Genome m1;
	//Genome m2;
	//m1.genome_encoding_2b2_int = { {1,0}, {2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},{9,0} };
	//m2.genome_encoding_2b2_int = { {9,0}, {3,0},{7,0},{8,0},{2,0},{6,0},{5,0},{1,0},{4,0} };
	//Encoding_Ass1.partiallyMappedCrossover2(m1, m2);

	// ############### Init Parameters
	unsigned int POPULATION_SIZE = 500;

	//// ############### INitialize the GA using the Encoding and parameters
	GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	// ############### SET GA PARAMETERS
	// --- parent selection
	GA.m_params.strategy = 1;
	GA.m_params.carry_over = 1;
	GA.m_params.selection_ratio = 0.5F;
	GA.m_params.window_size = 3;
	GA.m_params.replacement = true;
	GA.m_params.diversity_ratio = 0.2F;
	GA.m_params.purge_ratio = 0.3f;

	// --- Recombination parameters
	GA.m_params.crossoverProb = 0.7F;
	GA.m_params.allowFailures = false;
	GA.m_params.goalOffspringSize = POPULATION_SIZE;

	// --- Mutation parameters
	GA.m_params.mutationProb = 0.7F; // we should have higher mutation if diversity is low

	// --- Termination Condition Parameters
	GA.m_params.maxGeneration = 20000;
	GA.m_params.maxRuntime = 600; // seconds
	GA.m_params.targetFitness = 10;

	GA.runGA();

	return 0;
}