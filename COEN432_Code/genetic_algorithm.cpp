#include "genetic_algorithm.h"

GeneticAlgorithm::GeneticAlgorithm(GAEncoding *encoding, int population_size):
	generation(1)
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


void GeneticAlgorithm::recombination(float crossoverProb, int goalOffspringSize, bool allowfailures)
{
	watch.Start();
	m_encoding->recombination(crossoverProb, goalOffspringSize, allowfailures);
	genetic_algo_log() << "Recombination time using crossover prob [" << crossoverProb << "]: " << watch.Stop() << std::endl;
}

void GeneticAlgorithm::mutation(float mutationProb)
{
	watch.Start();
	m_encoding->mutation(mutationProb);
	genetic_algo_log() << "Mutation time using mutationProb [" << mutationProb << "]: " << watch.Stop() << std::endl;
}

void GeneticAlgorithm::survivorSelection()
{
	watch.Start();
	m_encoding->survivorSelection();
	genetic_algo_log() << "Survivor selection time " << watch.Stop() << std::endl;
}

bool GeneticAlgorithm::terminationConditions(int currentGen, int maxGeneration, double currRuntime, double maxRuntime, int targetFitness)
{
	watch.Start();
	bool val = m_encoding->terminationConditions(currentGen, maxGeneration, currRuntime, maxRuntime, targetFitness);
	genetic_algo_log() << "Termination calculation time " << watch.Stop() << std::endl;
	return val;
}

void GeneticAlgorithm::printParameters()
{
	genetic_algo_log() << "The GA algorithm has the following parameters: \n"
		<< "### Parent Selection ### \n"
		<< "strategy: " << params.strategy << std::endl
		<< "carry_over: " << params.carry_over << std::endl
		<< "selection_ration: " << params.selection_ratio << std::endl
		<< "window_size: " << params.window_size << std::endl
		<< "replacement: " << params.replacement << std::endl
		<< std::endl
		<< "### Recombination Parameters ### \n"
		<< "crossoverProb: " << params.crossoverProb << std::endl
		<< "allowFailures: " << params.allowFailures << std::endl
		<< std::endl
		<< "### Termination Condition Parameters ###\n"
		<< "maxGeneration: " << params.maxGeneration << std::endl
		<< "maxRuntime: " << params.maxRuntime << std::endl
		<< "targetFitness: " << params.targetFitness << std::endl
		;
}

void GeneticAlgorithm::runGA()
{

	printParameters();
	timeElapsedTimer.Start();
	
	// Run the GA loop as long as the terminationCondition does not evaluate to true
	while (!terminationConditions(generation,
		params.maxGeneration,
		timeElapsedTimer.Stop(),
		params.maxRuntime,
		params.targetFitness)) 
	{

		// Pick parents from the current population
		genetic_algo_log() << "Starting parent selection for generation: " << generation << std::endl;
		parentSelection(params.strategy,
			params.carry_over,
			params.selection_ratio,
			params.window_size,
			params.replacement);

		// Apply variation operators in order to create offspring
		genetic_algo_log() << "Starting recombination procedure..." << std::endl;
		recombination(params.crossoverProb, params.goalOffspringSize, params.allowFailures);

		genetic_algo_log() << "Starting mutation procedure... " << std::endl;
		mutation(params.mutationProb);

		// Select the survivors
		genetic_algo_log() << "Starting survivor selection... " << std::endl;
		survivorSelection();

		std::cout << m_encoding->m_elite[0].getFitness() << std::endl;
		// Increment the number of passed generations
		generation++;
	}

}