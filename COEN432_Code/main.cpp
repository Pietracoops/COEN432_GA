#include <iostream>
#include "helper_functions.h"
#include "encoding.h"
#include "genetic_algorithm.h"

int main()
{
	// ############### Initialize the Encoding based on the input file
	GAEncoding_Ass1 Encoding_Ass1("Ass1/Ass1Input.txt");
	//GAEncoding* Encoding = &Encoding_Ass1;

	//// ############### Init Parameters
	//unsigned int POPULATION_SIZE = 1000;

	//// ############### INitialize the GA using the Encoding and parameters
	//GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	//// ############### SET GA PARAMETERS
	//// --- parent selection
	//GA.m_params.strategy = 1;
	//GA.m_params.carry_over = 1;
	//GA.m_params.selection_ratio = 0.5F;
	//GA.m_params.window_size = 3;
	//GA.m_params.replacement = true;

	//// --- Recombination parameters
	//GA.m_params.crossoverProb = 0.8F;
	//GA.m_params.allowFailures = false;
	//GA.m_params.goalOffspringSize = POPULATION_SIZE;

	//// --- Mutation parameters
	//GA.m_params.mutationProb = 0.1F;

	//// --- Termination Condition Parameters
	//GA.m_params.maxGeneration = 5;
	//GA.m_params.maxRuntime = 600; // seconds
	//GA.m_params.targetFitness = 10;

	//// Run the GA
	//GA.runGA();

	//// Collect outputs and analyze
	std::vector<std::vector<int>> vec1{ {1, 0}, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 }, { 7, 0 }, { 8, 0 }, { 9, 0 } };
	Genome test1(vec1);

	std::vector<std::vector<int>> vec2{ {9, 0}, { 3, 0 }, { 7, 0 }, { 8, 0 }, { 2, 0 }, { 6, 0 }, { 5, 0 }, { 1, 0 }, { 4, 0 } };
	Genome test2(vec2);


	std::vector<Genome> res = Encoding_Ass1.partiallyMappedCrossover(test1, test2);

	std::cout << "vec1: ";
	for (int i = 0; i < vec1.size(); i++) {
		std::cout << vec1[i][0];
	}
	std::cout << std::endl;

	std::cout << "vec2: ";
	for (int i = 0; i < vec2.size(); i++) {
		std::cout << vec2[i][0];
	}
	std::cout << std::endl;

	std::cout << "offspring1: ";
	for (int i = 0; i < res[0].getEncoding().size(); i++) {
		std::cout << res[0].getEncoding()[i][0];
	}
	std::cout << std::endl;

	std::cout << "offspring2: ";
	for (int i = 0; i < res[1].getEncoding().size(); i++) {
		std::cout << res[1].getEncoding()[i][0];
	}
	std::cout << std::endl;




	return 0;
}