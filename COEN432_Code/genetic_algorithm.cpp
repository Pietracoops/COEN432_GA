// Christopher Neves: 27521979 / Massimo Pietracupa: 27313683
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
	// Destructor
}

void GeneticAlgorithm::initializePopulation(int population_size)
{
	m_watch.Start();
	m_encoding->initializaPopulation(population_size); // Initialize the population randomly and also collect their fitness
	genetic_algo_log() << "Initializing Population with size [" << population_size << "]" << " - elapsed time: " << m_watch.Stop() << std::endl;
}

void GeneticAlgorithm::parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement, float randomness)
{
	m_watch.Start();
	m_encoding->parentSelection(strategy, carry_over, selection_ratio, window_size, replacement, randomness);
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

void GeneticAlgorithm::returnEliteFenotype()
{
	// Export to file
	Genome elite = m_encoding->getEliteFromFile(m_population_log_name);
	std::string elite_phenotype = m_encoding->genotypeToPhenotype(elite);
	std::string savename = m_population_log_name + "_Elite.txt";
	savePhenotypeToFile(elite_phenotype, savename);
}

void GeneticAlgorithm::runGA(std::string population_file)
{
	// This is the main GA LOOP

	if (!population_file.empty())
	{
		// Load population from file if we want to continue a previous run
		m_encoding->loadPopulation(population_file, m_params.population_size);
	}

	// Print parameters to the logger to identify configuration of last run
	Logger(printParameters());

	// Start the timer
	m_time_elapsed_timer.Start();
	
	// Set the stagnation window
	stats.setStagnationWindow(m_params.stagnation_check);

	// Run the GA loop so long as the terminationCondition does not evaluate to true
	while (!terminationConditions(m_generation,
		m_params.maxGeneration,
		m_time_elapsed_timer.Stop(),
		m_params.maxRuntime,
		m_params.targetFitness)) 
	{

		genetic_algo_log() << "======================== GENERATION "<< m_generation << " =============================== " << std::endl;

		// Begin the loop with parent selection
		genetic_algo_log() << "Starting parent selection for generation: " << m_generation << std::endl;
		parentSelection(m_params.strategy,
			m_params.carry_over,
			m_params.selection_ratio,
			m_params.window_size,
			m_params.replacement,
			m_params.randomness);

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

		// Apply mutations to the population
		genetic_algo_log() << "Starting mutation procedure... " << std::endl;
		mutation(m_params.mutationProb, m_params.accelerated);

		// Select the survivors
		genetic_algo_log() << "Starting survivor selection... " << std::endl;
		survivorSelection(m_params.survivorpolicy, m_params.survivorsize);

		// Create string to log out to LOG
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

		// Dynamically Tunee hyperparameters based on fitness
		if (m_params.dynamic_hyper_parameters)
		{
			if (stats.stagnation_detected)
			{
				//stats.tuneParameter(m_params.crossoverProb, 0.8f, 0.0f, 0.9f); // Experimenting with a decreasing crossoverprob
				//stats.tuneParameter(m_params.mutationProb, 1.1f, 0.2f, 1.0f);  // Experimenting with an increasing mutationProb
				stats.tuneParameter(m_params.crossoverProb, 1.1f, 0.0f, 0.9f);	 // Experimenting with an increasing crossoverprob
				stats.tuneParameter(m_params.mutationProb, 0.8f, 0.2f, 1.0f);	 // Experimenting with a decreasing mutationprob
				stats.tuneParameter(m_params.randomness, 1.05f, 0.0f, 0.2f);	 // Experimenting with an increasing randomness
				stats.m_stagnation_modified_countdown = stats.m_stagnation_countdown_val; // Note down the generation where we tuned
			}
		}


		// Save population every number of generations
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

	// Save the population after generation loop has been complete
	if (m_params.save_population)
	{
		m_encoding->savePopulation();
	}
}