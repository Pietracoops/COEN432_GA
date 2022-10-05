#include <iostream>
#include "helper_functions.h"
#include "encoding.h"
#include "genetic_algorithm.h"

int main()
{
	
	// PARAMETERS
	unsigned int POPULATION_SIZE = 10000;

	//// Prof's C++ functions for validation
	//ProfTest("Ass1/Ass1Input.txt");

	// Initialize the Encoding based on the input file
	GAEncoding_Ass1 Encoding_Ass1("Ass1/Ass1Input.txt");

	GAEncoding* Encoding = &Encoding_Ass1;
	// INitialize the GA using the Encoding and parameters
	GeneticAlgorithm GA(Encoding, POPULATION_SIZE);

	

	return 0;
}