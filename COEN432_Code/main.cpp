#include <iostream>
#include "helper_functions.h"
#include "encoding.h"
#include "genetic_algorithm.h"

int main()
{
	// ############### Initialize the Encoding based on the input file
	GAEncoding_Ass1 Encoding_Ass1("Ass1/Ass1Input.txt");
	GAEncoding* Encoding = &Encoding_Ass1;

	// ############### Init Parameters
	unsigned int POPULATION_SIZE = 10000;

	// ############### INitialize the GA using the Encoding and parameters
	GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	// ############### SET GA PARAMETERS
	// --- parent selection
	GA.m_params.strategy = 1;
	GA.m_params.carry_over = 1;
	GA.m_params.selection_ratio = 0.5F;
	GA.m_params.window_size = 3;
	GA.m_params.replacement = true;

	// --- Recombination parameters
	GA.m_params.crossoverProb = 0.0F;
	GA.m_params.allowFailures = true;
	GA.m_params.goalOffspringSize = POPULATION_SIZE;

	// --- Mutation parameters
	GA.m_params.mutationProb = 1.0F;

	// --- Termination Condition Parameters
	GA.m_params.maxGeneration = 20000;
	GA.m_params.maxRuntime = 600; // seconds
	GA.m_params.targetFitness = 10;

	// Run the GA
	GA.runGA();

	// Collect outputs and analyze
	

	return 0;
}