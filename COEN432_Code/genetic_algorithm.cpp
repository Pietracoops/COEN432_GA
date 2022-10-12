#include "genetic_algorithm.h"

GeneticAlgorithm::GeneticAlgorithm(GAEncoding *encoding, int population_size, std::string log_path):
	m_generation(1)
{
	if (log_path.empty())
	{
		std::filesystem::path log_dir = std::filesystem::current_path() / "log";
		log_path = log_dir.string();
	}

	LOG_PATH = log_path + ("/log_" + getCurrentDateTime("date") + ".log");
	std::filesystem::create_directories(log_path);

	// Store encoding locally
	m_encoding = encoding; 

	// Init the population
	initializePopulation(population_size);

}


GeneticAlgorithm::~GeneticAlgorithm()
{

}

void GeneticAlgorithm::initializePopulation(int population_size)
{
	m_watch.Start();
	// This is where we specify our initialization parameters
	genetic_algo_log() << "Initializing Population with size [" << population_size << "]" << std::endl;

	// Initialize the population randomly and also collects their fitness
	m_encoding->initializaPopulation(population_size);

	genetic_algo_log() << "elapsed time: " << m_watch.Stop() << std::endl;
}


void GeneticAlgorithm::parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement, float randomness, float diversity_ratio, float purge_ratio)
{
	m_watch.Start();
	m_encoding->parentSelection(strategy, carry_over, selection_ratio, window_size, replacement, randomness, diversity_ratio, purge_ratio);
	genetic_algo_log() << "Parent Selection time using strategy [" << strategy << "]: " << m_watch.Stop() << std::endl;
}


void GeneticAlgorithm::recombination(float crossoverProb, int goalOffspringSize, bool skipCrossover)
{
	m_watch.Start();
	m_encoding->recombination(crossoverProb, goalOffspringSize, skipCrossover);
	genetic_algo_log() << "Recombination time using crossover prob [" << crossoverProb << "]: " << m_watch.Stop() << std::endl;
}

void GeneticAlgorithm::mutation(float mutationProb, bool accelerated)
{
	m_watch.Start();
	m_encoding->mutation(mutationProb, accelerated);
	genetic_algo_log() << "Mutation time using mutationProb [" << mutationProb << "]: " << m_watch.Stop() << std::endl;
}

void GeneticAlgorithm::survivorSelection(int policy, int survivorSize)
{
	m_watch.Start();
	m_encoding->survivorSelection(policy, survivorSize);
	genetic_algo_log() << "Survivor selection time " << m_watch.Stop() << std::endl;
}

bool GeneticAlgorithm::terminationConditions(int currentGen, int maxGeneration, double currRuntime, double maxRuntime, int targetFitness)
{
	m_watch.Start();
	bool val = m_encoding->terminationConditions(currentGen, maxGeneration, currRuntime, maxRuntime, targetFitness);
	genetic_algo_log() << "Termination calculation time " << m_watch.Stop() << std::endl;
	return val;
}

std::string GeneticAlgorithm::printParameters()
{
	std::string output;

	output = "\n===========================================================\n";
	output += "The GA algorithm has the following parameters: \n";
	output += "Population Size: " + std::to_string(m_encoding->m_population.size()) + '\n';
	output += "### Parent Selection ### \n";
	output += "strategy: " + std::to_string(m_params.strategy) + '\n';
	output += "carry_over: " + std::to_string(m_params.carry_over) + '\n';
	output += "selection_ration: " + std::to_string(m_params.selection_ratio) + '\n';
	output += "window_size: " + std::to_string(m_params.window_size) + '\n';
	output += "replacement: " + std::to_string(m_params.replacement) + '\n';
	output += "randomness: " + std::to_string(m_params.randomness) + '\n';
	output += "diversity_ratio: " + std::to_string(m_params.diversity_ratio) + '\n';
	output += "purge_ratio: " + std::to_string(m_params.purge_ratio) + '\n';
	output += "### Recombination Parameters ### \n";
	output += "crossoverProb: " + std::to_string(m_params.crossoverProb) + '\n';
	output += "skipCrossover: " + std::to_string(m_params.skipCrossover) + '\n';
	output += "mutationProb: " + std::to_string(m_params.mutationProb) + '\n';
	output += "accelerated: " + std::to_string(m_params.accelerated) + '\n';
	output += "### Termination Condition Parameters ###\n";
	output += "maxGeneration: " + std::to_string(m_params.maxGeneration) + '\n';
	output += "maxRuntime: " + std::to_string(m_params.maxRuntime) + '\n';
	output += "targetFitness: " + std::to_string(m_params.targetFitness) + '\n';
	output += "===========================================================\n";

	genetic_algo_log() << "The GA algorithm has the following parameters: \n"
		<< "### Parent Selection ### \n"
		<< "strategy: " << m_params.strategy << std::endl
		<< "carry_over: " << m_params.carry_over << std::endl
		<< "selection_ration: " << m_params.selection_ratio << std::endl
		<< "window_size: " << m_params.window_size << std::endl
		<< "replacement: " << m_params.replacement << std::endl
		<< std::endl
		<< "### Recombination Parameters ### \n"
		<< "crossoverProb: " << m_params.crossoverProb << std::endl
		<< "skipCrossover: " << m_params.skipCrossover << std::endl
		<< std::endl
		<< "### Termination Condition Parameters ###\n"
		<< "maxGeneration: " << m_params.maxGeneration << std::endl
		<< "maxRuntime: " << m_params.maxRuntime << std::endl
		<< "targetFitness: " << m_params.targetFitness << std::endl;

	return output;
}

void GeneticAlgorithm::runGA(std::string population_file)
{

	if (!population_file.empty())
	{
		m_encoding->loadPopulation(population_file, m_params.population_size);
	}

	Logger(printParameters());
	m_time_elapsed_timer.Start();
	

	// Set the stagnation window
	stats.setStagnationWindow(m_params.stagnation_check);

	// Run the GA loop as long as the terminationCondition does not evaluate to true
	while (!terminationConditions(m_generation,
		m_params.maxGeneration,
		m_time_elapsed_timer.Stop(),
		m_params.maxRuntime,
		m_params.targetFitness)) 
	{

		genetic_algo_log() << "======================== GENERATION "<< m_generation << " =============================== " << std::endl;

		// Pick parents from the current population
		genetic_algo_log() << "Starting parent selection for generation: " << m_generation << std::endl;
		parentSelection(m_params.strategy,
			m_params.carry_over,
			m_params.selection_ratio,
			m_params.window_size,
			m_params.replacement,
			m_params.randomness,
			m_params.diversity_ratio,
			m_params.purge_ratio);

		// Check for stagnation
		stats.checkStagnation(m_encoding->m_max_fitness, m_generation);

		// Inject parents if stagnated
		if (stats.stagnation_detected && m_params.inject_parents)
		{
			genetic_algo_log() << "Stagnation detected. Injecting " << m_params.random_parent_proportion * m_encoding->m_parents.size() << " parents.";
			m_encoding->injectParents(m_params.random_parent_proportion);
		}

		// Apply variation operators in order to create offspring
		genetic_algo_log() << "Starting recombination procedure..." << std::endl;
		recombination(m_params.crossoverProb, m_params.goalOffspringSize, m_params.skipCrossover);

		genetic_algo_log() << "Starting mutation procedure... " << std::endl;
		mutation(m_params.mutationProb, m_params.accelerated);

		// Select the survivors
		genetic_algo_log() << "Starting survivor selection... " << std::endl;
		survivorSelection(m_params.survivorpolicy, m_params.survivorsize);


		std::string logger_str = "GENERATION: " + std::to_string(m_generation)
			+ " ;AVERAGE_FITNESS: " + std::to_string(m_encoding->getAverageFitness(m_encoding->m_population))
			+ " ;MAX_FITNESS " + std::to_string(m_encoding->m_max_fitness)
			+ " ;MEDIAN_FITNESS " + std::to_string(m_encoding->m_med_fitness)
			+ " ;MIN_FITNESS " + std::to_string(m_encoding->m_min_fitness)
			+ " ;STAGNATION_DETECTED " + std::to_string(stats.stagnation_detected)
			+ " ;MUTATION_PROB " + std::to_string(m_params.mutationProb)
			+ " ;CROSSOVER_PROB " + std::to_string(m_params.crossoverProb)
			+ " ;RANDOMNESS " + std::to_string(m_params.randomness);

		if (m_encoding->m_elite.size() != 0)
		{
			logger_str += " ;ELITE GENOME " + m_encoding->m_elite[0].getGenomeString();
		}
		Logger(logger_str);


		genetic_algo_log() << "========================== END OF GENERATION =============================== " << std::endl;

		// ============================ STATS =================================
		stats.m_max_fitness.push_back(m_encoding->m_max_fitness);
		stats.m_min_fitness.push_back(m_encoding->m_min_fitness);

		// Tuneable hyperparameters
		if (m_params.dynamic_hyper_parameters)
		{
			if (stats.stagnation_detected)
			{
				//stats.tuneParameter(m_params.crossoverProb, 0.8f, 0.0f, 0.9f);
				//stats.tuneParameter(m_params.mutationProb, 1.1f, 0.2f, 1.0f);
				stats.tuneParameter(m_params.crossoverProb, 1.1f, 0.0f, 0.9f);
				stats.tuneParameter(m_params.mutationProb, 0.8f, 0.2f, 1.0f);
				stats.tuneParameter(m_params.randomness, 1.05f, 0.0f, 0.2f);
				stats.m_stagnation_modified_countdown = stats.m_stagnation_countdown_val; // Note down the generation where we tuned
			}
		}


		// Save network every number of generations
		if (m_params.save_every_x_generation)
		{
			if (m_generation % m_params.save_every_x_generation_val == 0)
			{
				m_encoding->savePopulation();
			}
		}

		// Increment the number of passed generations
		m_generation++;
	}


	if (m_params.save_population)
	{
		m_encoding->savePopulation();
	}
}


//bool NetworkStatistics::checkStagnation(int generation_range, int generation, int breath)
//{
//	
//	// Check Stagnation
//	if (generation_range > m_max_fitness.size())
//	{
//		stagnation_detected = false;
//		return false;
//	}
//
//	if (m_max_fitness[(m_max_fitness.size() - generation_range)] != m_max_fitness.back())
//	{
//		stagnation_detected = false;
//		m_stagnation_modified_countdown = -1;
//		return false;
//	}
//
//	if (m_stagnation_modified_countdown > 0)
//	{
//		return false;
//	}
//
//	stagnation_detected = true;
//	return true;
//	
//}

bool NetworkStatistics::checkStagnation(int currentFitness, int generation)
{

	if (currentFitness == previousFitness)
	{
		fitnessRepeats++;
	}
	else {
		fitnessRepeats = 0;
	}

	previousFitness = currentFitness;

	if (fitnessRepeats > stagnation_window)
	{
		stagnation_detected = true;
	}
	else {
		stagnation_detected = false;
	}

	return stagnation_detected;
	
}

void NetworkStatistics::tuneParameter(float& parameter, const float tune, const float clamp_min, const float clamp_max)
{
	if (parameter == 0.0f && tune > 1.0f)
	{
		parameter = 0.1f;
	}
	if (parameter > 0.0f && tune > 1.0f)
	{
		parameter += 0.05f;
	}

	parameter *= tune;
	parameter = std::clamp(parameter, clamp_min, clamp_max);
}

NetworkStatistics::NetworkStatistics()
{
	stagnation_detected = false;
	m_stagnation_modified_countdown = -1;
}

void NetworkStatistics::setStagnationWindow(int stagnation_window)
{
	this->stagnation_window = stagnation_window;
}