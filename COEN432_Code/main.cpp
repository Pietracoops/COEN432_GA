#include <iostream>
#include "helper_functions.h"
#include "encoding.h"
#include "genetic_algorithm.h"

int main()
{
	


	// PARAMETERS
	unsigned int POPULATION_SIZE = 10;

	//// Prof's C++ functions for validation
	//ProfTest("Ass1/Ass1Input.txt");

	// Initialize the Encoding based on the input file
	GAEncoding_Ass1 Encoding_Ass1("Ass1/Ass1Input.txt");

	GAEncoding* Encoding = &Encoding_Ass1;
	// INitialize the GA using the Encoding and parameters
	GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	//GA.parentSelection(0, 2, 0.5, 3, false);
	GA.parentSelection(1, 2, 0.5, 3, true);

	

	return 0;
}