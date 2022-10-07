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


void GeneticAlgorithm::parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement)
{
	watch.Start();
	m_encoding->parentSelection(strategy, carry_over, selection_ratio, window_size, replacement);
	genetic_algo_log() << "Parent Selection time using strategy [" << strategy << "]: " << watch.Stop() << std::endl;
}


void GeneticAlgorithm::recombination(float crossoverProb)
{
	m_encoding->recombination(crossoverProb);
}


void GeneticAlgorithm::survivorSelection()
{
	m_encoding->survivorSelection();
}
