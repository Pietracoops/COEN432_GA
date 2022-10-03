#include "genetic_algorithm.h"

GeneticAlgorithm::GeneticAlgorithm(GAEncoding_Ass1 encoding, int population_size)
{
	auto start = std::chrono::steady_clock::now();
	// This is where we specify our initialization parameters
	genetic_algo_log() << "Initializing Population with size [" << population_size << "]" << std::endl;

	// Initialize the population randomly and also collects their fitness
	m_encoding = encoding;
	m_encoding.initializaPopulation(population_size);


	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	genetic_algo_log() << "elapsed time: " << elapsed_seconds.count() << std::endl;
}


GeneticAlgorithm::~GeneticAlgorithm()
{


}