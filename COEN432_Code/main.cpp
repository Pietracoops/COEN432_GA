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
	unsigned int POPULATION_SIZE = 65;

	//// ############### INitialize the GA using the Encoding and parameters
	GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	// ############### SET GA PARAMETERS
	// --- parent selection
	GA.m_params.strategy = 1;
	GA.m_params.carry_over = 1;
	GA.m_params.selection_ratio = 0.6F;
	GA.m_params.window_size = 3;
	GA.m_params.replacement = false;
	GA.m_params.randomness = 0.0f;
	GA.m_params.diversity_ratio = 0.1F;
	GA.m_params.purge_ratio = 0.25f;

	// --- Recombination parameters

	GA.m_params.crossoverProb = 0.3F;
	GA.m_params.skipCrossover = false;
	GA.m_params.goalOffspringSize = POPULATION_SIZE;

	// --- Mutation parameters
	GA.m_params.mutationProb = 1.0F; // we should have higher mutation if diversity is low

	// --- Termination Condition Parameters
	GA.m_params.maxGeneration = 1000;
	GA.m_params.maxRuntime = 600; // seconds
	GA.m_params.targetFitness = 10;

	// --- Stats
	GA.m_params.dynamic_hyper_parameters = false;
	GA.m_params.stagnation_check = 18;
	GA.m_params.stagnation_breath = 8;

	GA.m_params.save_population = true;

	GA.runGA();


	return 0;
}