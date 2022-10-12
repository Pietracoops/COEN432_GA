#include <iostream>
#include "helper_functions.h"
#include "encoding.h"
#include "genetic_algorithm.h"

#include <numeric>

int main()
{
	// ############### Initialize the Encoding based on the input file


	GAEncoding_Ass1 Encoding_test("Ass1/test.txt");
	Genome mass = Encoding_test.m_original_genome;
	mass.setFitness(Encoding_test.fitnessOfGenome(mass));


	GAEncoding_Ass1 Encoding_Ass1("Ass1/Ass1Input.txt");
	GAEncoding* Encoding = &Encoding_Ass1;

	// ############### Init Parameters
	unsigned int POPULATION_SIZE = 200;


	//// ############### INitialize the GA using the Encoding and parameters
	GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	// ############### SET GA PARAMETERS
	// --- parent selection
	GA.m_params.population_size = POPULATION_SIZE;
	GA.m_params.strategy = 1;
	GA.m_params.carry_over = 1;
	GA.m_params.selection_ratio = 0.6F; // Selection ratio has no effect on resulting parent size if using tournament, use window size instead
	GA.m_params.window_size = 3;
	GA.m_params.replacement = false;
	GA.m_params.randomness = 0.0f;
	GA.m_params.diversity_ratio = 0.0F;
	GA.m_params.purge_ratio = 0.0f;

	// --- Recombination parameters

	GA.m_params.crossoverProb = 0.9F;
	GA.m_params.skipCrossover = false;
	GA.m_params.goalOffspringSize = 1.1 * POPULATION_SIZE;

	// --- Mutation parameters
	GA.m_params.mutationProb = 0.07F; // we should have higher mutation if diversity is low
	GA.m_params.accelerated = true;

	// Survivor selection parameters
	GA.m_params.survivorpolicy = 0; // 0 is ufromgamma, 1 is uplusgamma, 2 is uplusgamma fuds
	GA.m_params.survivorsize = POPULATION_SIZE;

	// Stagnation handling
	GA.m_params.inject_parents = false;
	GA.m_params.random_parent_proportion = 0.1F;


	// --- Termination Condition Parameters
	GA.m_params.maxGeneration = 1000000;
	GA.m_params.maxRuntime = 600; // seconds
	GA.m_params.targetFitness = 10;

	// --- Stats
	GA.m_params.dynamic_hyper_parameters = false;
	GA.m_params.stagnation_check = 50;
	GA.m_params.stagnation_breath = 8;

	GA.m_params.save_population = true;
	GA.m_params.save_every_x_generation = true;
	GA.m_params.save_every_x_generation_val = 250;


	GA.runGA("population_2022-10-11-15-56-08.txt");



	return 0;
}