#include "genetic_algorithm.h"

GeneticAlgorithm::GeneticAlgorithm(GAEncoding *encoding, int population_size)
{
	watch.Start();
	// This is where we specify our initialization parameters
	genetic_algo_log() << "Initializing Population with size [" << population_size << "]" << std::endl;

	// Initialize the population randomly and also collects their fitness
	m_encoding = encoding;
	m_encoding->initializaPopulation(population_size);

	
	genetic_algo_log() << "elapsed time: " << watch.Stop() << std::endl;
}


GeneticAlgorithm::~GeneticAlgorithm()
{

}


void GeneticAlgorithm::parentSelection()
{
	m_encoding->parentSelection();
}


void GeneticAlgorithm::recombination(float crossoverProb)
{
	m_encoding->recombination(crossoverProb);
}


void GeneticAlgorithm::survivorSelection()
{
	m_encoding->survivorSelection();
}