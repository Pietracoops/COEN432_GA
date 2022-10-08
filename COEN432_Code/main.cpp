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
	GA.params.strategy = 1;
	GA.params.carry_over = 1;
	GA.params.selection_ratio = 0.5F;
	GA.params.window_size = 3;
	GA.params.replacement = true;

	// --- Recombination parameters
	GA.params.crossoverProb = 0.8F;
	GA.params.allowFailures = false;
	GA.params.goalOffspringSize = POPULATION_SIZE;

	// --- Mutation parameters
	GA.params.mutationProb = 0.1F;

	// --- Termination Condition Parameters
	GA.params.maxGeneration = 5;
	GA.params.maxRuntime = 600; // seconds
	GA.params.targetFitness = 10;

	// Run the GA
	GA.runGA();

	// Collect outputs and analyze
	

	return 0;
}