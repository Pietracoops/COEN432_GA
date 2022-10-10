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
	unsigned int POPULATION_SIZE = 100;

	//// ############### INitialize the GA using the Encoding and parameters
	GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	// ############### SET GA PARAMETERS
	// --- parent selection
	GA.m_params.strategy = 1;
	GA.m_params.carry_over = 1;
	GA.m_params.selection_ratio = 0.6F;
	GA.m_params.window_size = 3;
	GA.m_params.replacement = false;
<<<<<<< Updated upstream
	GA.m_params.randomness = 0.0f;
=======
	GA.m_params.randomness = 0.2f;
>>>>>>> Stashed changes
	GA.m_params.diversity_ratio = 0.1F;
	GA.m_params.purge_ratio = 0.25f;

	// --- Recombination parameters
	GA.m_params.crossoverProb = 0.6F;
	GA.m_params.allowFailures = false;
	GA.m_params.goalOffspringSize = POPULATION_SIZE;

	// --- Mutation parameters
	GA.m_params.mutationProb = 1.0F; // we should have higher mutation if diversity is low

	// --- Termination Condition Parameters
	GA.m_params.maxGeneration = 3000;
	GA.m_params.maxRuntime = 600; // seconds
	GA.m_params.targetFitness = 10;

	// --- Stats
	GA.m_params.dynamic_hyper_parameters = false;
	GA.m_params.stagnation_check = 18;
	GA.m_params.stagnation_breath = 8;

	GA.m_params.save_population = true;

<<<<<<< Updated upstream
	GA.runGA();
=======
	GA.runGA("population_2022-10-10-12-08-35.txt");
>>>>>>> Stashed changes

	return 0;
}