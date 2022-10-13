// Christopher Neves: 27521979 / Massimo Pietracupa: 27313683
#include <iostream>
#include "helper_functions.h"
#include "encoding.h"
#include "genetic_algorithm.h"

#include <numeric>

int main()
{
	// ############### Initialize the Encoding based on the input file
	GAEncoding_Ass1 Encoding_Ass1("Ass1/Ass1Input.txt");
	GAEncoding* Encoding = &Encoding_Ass1;

	// ############### Init Parameters
	unsigned int POPULATION_SIZE = 200;

	//// ############### INitialize the GA using the Encoding and parameters
	GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	// ############### SET GA PARAMETERS
	// Descriptions for each parameter can be found in genetic_algorithm.h
	// 
	// --- parent selection
	GA.m_params.population_size = POPULATION_SIZE;
	GA.m_params.strategy = 1;
	GA.m_params.carry_over = 1;
	GA.m_params.selection_ratio = 0.6F; // Note: Selection ratio has no effect on resulting parent size if using tournament, use window size instead
	GA.m_params.window_size = 3;
	GA.m_params.replacement = false;
	GA.m_params.randomness = 0.0f;

	// --- Recombination parameters
	GA.m_params.crossoverProb = 0.7F;
	GA.m_params.skipCrossover = false;
	GA.m_params.goalOffspringSize = 2 * POPULATION_SIZE;

	// --- Mutation parameters
	GA.m_params.mutationProb = 0.05F;
	GA.m_params.accelerated = false;

	// --- Survivor selection parameters
	GA.m_params.survivorpolicy = 0; // Note: 0 is ufromgamma, 1 is uplusgamma, 2 is uplusgamma fuds
	GA.m_params.survivorsize = POPULATION_SIZE;

	// --- Stagnation handling
	GA.m_params.inject_parents = false;
	GA.m_params.random_parent_proportion = 0.1F;

	// --- Termination Condition Parameters
	GA.m_params.maxGeneration = 1000000;
	GA.m_params.maxRuntime = 600; // seconds
	GA.m_params.targetFitness = 112;

	// --- Stats
	GA.m_params.dynamic_hyper_parameters = false;
	GA.m_params.stagnation_check = 50;

	// --- Saving
	GA.m_params.save_population = true;
	GA.m_params.save_every_x_generation = true;
	GA.m_params.save_every_x_generation_val = 250;

	GA.runGA();
	GA.returnEliteFenotype();

	std::cout << "Program Terminated." << std::endl;
	return 0;
}