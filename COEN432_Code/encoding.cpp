// Christopher Neves: 27521979 / Massimo Pietracupa: 27313683
#include "encoding.h"

// TILE OBJECTS

GAEncoding_Ass1::Tile::Tile(std::string constructor)
{
	top		= constructor[0];
	right	= constructor[1];
	bottom	= constructor[2];
	left	= constructor[3];
	
}

void GAEncoding_Ass1::Tile::rotate(unsigned int rot_value)
{
	std::string current[4] = { top, right, bottom, left };

	top		= current[(0 + rot_value) % 4];
	right	= current[(1 + rot_value) % 4];
	bottom	= current[(2 + rot_value) % 4];
	left	= current[(3 + rot_value) % 4];
}

void GAEncoding_Ass1::Tile::printTile()
{
	std::cout << "Top = " << top << " Right = " << right << " Bottom = " << bottom << " Left = " << left << std::endl;
}


// ASS1 ENCODINGS

std::vector<std::vector<int>> GAEncoding_Ass1::returnGenome()
{
	return m_original_genome;
}

std::vector<int> GAEncoding_Ass1::fitnessOfGenome(const Genome& genome)
{
	// 8x8 box - meaning 

	std::vector<std::vector<int>> genes = genome.getEncoding();
	std::vector<int> output; // [0] fitness [1] row mismatches [2] col mismatches

	// Calculate rows
	int row_mismatches = 0;
	int total = 0;

	// Check mismatches between tiles in same row
	for (unsigned int i = 1; i < genome.getSize(); i++)
	{
		if (i % WIDTH == 0) 
		{
			continue;
		}
		Tile T1 = m_map_index[genes[i - 1][0]];
		T1.rotate(genes[i - 1][1]);
		Tile T2 = m_map_index[genes[i][0]];
		T2.rotate(genes[i][1]);
		if (T1.right != T2.left)
		{
			row_mismatches++;
		}
		total++;
	}

	// Check mismatches between tiles in columns
	int col_mismatches = 0;
	for (unsigned int i = 0; i < genome.getSize() - WIDTH; i++)
	{
		Tile T1 = m_map_index[genes[i][0]];
		T1.rotate(genes[i][1]);
		Tile T2 = m_map_index[genes[i + WIDTH][0]];
		T2.rotate(genes[i + WIDTH][1]);
		if (T1.bottom != T2.top)
		{
			col_mismatches++;
		}
		total++;
	}

	// Revert the fitness so that it increases (Fitness = number of successful edges)
	output.push_back(MAX_MISMATCHES - (col_mismatches + row_mismatches));
	output.push_back(row_mismatches);
	output.push_back(col_mismatches);
	return output;
}

std::vector<std::vector<std::vector<int>>> GAEncoding_Ass1::returnRandomizedGenome(const unsigned int number_of_genomes)
{
	std::vector<std::vector<std::vector<int>>> output;

	for (unsigned int i = 0; i < number_of_genomes; i++)
	{
		std::vector<std::vector<int>> randomized_genome = shuffleVector(m_original_genome);
		output.push_back(randomized_genome);
	}
	return output;
}

void GAEncoding_Ass1::initializaPopulation(const unsigned int number_of_genomes)
{
	// Set the population size attribute of the encoding
	starting_pop_size = number_of_genomes;

	for (unsigned int i = 0; i < number_of_genomes; i++)
	{
		m_population.push_back(returnRandomlyInitializedGenome());
	}
	
}

void GAEncoding_Ass1::encodeToMap(std::vector<std::vector<std::string>> input)
{
	// Store the input into the m_original_genome map
	unsigned int tile_index = 0;
	for (unsigned int i = 0; i < input.size(); i++)
	{
		for (unsigned int j = 0; j < input[0].size(); j++)
		{
			std::vector<int> tile_couple;
			Tile puzzle_tile(input[i][j]);
			m_map_index[tile_index] = puzzle_tile;
			tile_couple.push_back(tile_index);	// Index
			tile_couple.push_back(0);			// Rotation
			m_original_genome.push_back(tile_couple);
			tile_index++;
		}
	}
}

GAEncoding_Ass1::GAEncoding_Ass1(std::string file_name)
{
	// CONSTRUCTOR
	// Read input file
	std::vector<std::string> vs_file = void_file;
	if (!file_name.empty())
	{
		encoding_log() << "Using File: " << file_name << std::endl;
		vs_file = readFileIntoMemory(file_name);
	}
	
	std::vector<std::vector<std::string>> puzzle_box;

	for (unsigned int i = 0; i < vs_file.size(); i++)
	{
		// Loop through each row of file and delimit according to spaces
		std::stringstream currentLine(vs_file[i]);
		std::string currentNumber;
		std::vector<std::string> row;
		while (std::getline(currentLine, currentNumber, ' '))
		{
			row.push_back(currentNumber);
		}
		
		puzzle_box.push_back(row);
	}

	encodeToMap(puzzle_box);

	// Generate a random seed
	seed = rd() ^ ((std::mt19937::result_type)std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()).count() + (std::mt19937::result_type)
			std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	gen_mt.seed(seed);

	// Initialize Fitness Trackers
	m_max_fitness = 0;
	m_med_fitness = 0;
	m_min_fitness = 0;
}

GAEncoding_Ass1::~GAEncoding_Ass1()
{
	// Destructor
}

void GAEncoding_Ass1::parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement, float randomness)
{
	// Two types of parent selection
	// 1: Generational: Each individual survives for exactly one generation - set of parents completely replaced
	// 2: Steady-State: One offspring is generated per generation - one member of population replaced

	// Generation Gap is the input into parent selection where 1.0 is Generational and 1/pop_size is Steady-State

	// The process
	// 1. clear parents
	m_parents.clear();
	m_elite.clear();

	// 2. Sort according to fitness
	std::sort(m_population.begin(), m_population.end(), [](const Genome& lhs, const Genome& rhs){return lhs.getFitness() > rhs.getFitness(); });

	m_max_fitness = m_population[0].getFitness();
	m_med_fitness = m_population[m_population.size()/2].getFitness();
	m_min_fitness = m_population[m_population.size() - 1].getFitness();

	// 3. append the carry over to parents + remove them from original population list (ELITISM)
	for (unsigned int i = 0; i < carry_over; i++)
	{
		m_elite.push_back(m_population[0]);			// Store the first element in parents
		m_population.erase(m_population.begin());	// Erase the first element in population
	}

	// 4. Call Fitness Proportionate if selected
	std::vector<Genome> parents;
	if (strategy == 0)
	{
		parents = parentSelectionFitnessProportionate(m_population, selection_ratio);
	}
	// 4b. Call Tournament if selected
	else if (strategy == 1)
	{
		parents = parentSelectionTournament(m_population, selection_ratio, window_size, replacement);
	}
	
	// Apply Randomness to the new parents
	if (randomness > 0.0) 
	{
		unsigned int num_random_gen = (int)(parents.size() * randomness);
		// Parents is a shuffled array so we'll just pop out the last ones in array
		// and replace them with random initializations of the genome
		parents.erase(parents.end() - num_random_gen, parents.end());
		for (unsigned int i = 0; i < num_random_gen; i++)
		{
			parents.push_back(returnRandomlyInitializedGenome());
		}
	}

	// Complete
	m_parents = parents;
	
}

std::vector<Genome> GAEncoding_Ass1::parentSelectionFitnessProportionate(std::vector<Genome> population, float selection_ratio)
{
	std::vector<Genome> parents;

	// Find total 
	unsigned int max = 0;
	for (unsigned int i = 0; i < population.size(); i++)
	{
		max += population[i].getFitness();
	}

	unsigned int genomes_to_pick = (unsigned int)(selection_ratio * population.size());

	while (genomes_to_pick != 0)
	{
		// Pick a value within the interval 0 - max
		std::uniform_int_distribution<unsigned int> distribution(0, max);
		unsigned int pick = distribution(gen_mt);

		unsigned int current = 0;
		for (unsigned int i = 0; i < population.size(); i++)
		{
			// Roulette style picking
			current += population[i].getFitness();
			if (current > pick)
			{
				parents.push_back(population[i]);
				max -= population[i].getFitness();
				m_population.erase(m_population.begin() + i);
				break;
			}
		}
		genomes_to_pick--;
	}

	return parents;
}

std::vector<Genome> GAEncoding_Ass1::parentSelectionTournament(std::vector<Genome> population, float selection_ratio, uint32_t window_size, bool replacement)
{

	// Initialization
	std::vector<Genome> parents;			// Vector to store resulting parents
	std::vector<Genome> population_copy;	// Vector used to scramble parents
	uint32_t genomes_to_pick = (uint32_t)(selection_ratio * population.size()); // Number of genomes to select from population

	// Shuffle the population
	population_copy = shuffleVector(population);


	// No Replacement
	if (!replacement)
	{
		unsigned int index = 0;
		for (uint32_t i = 0; i < genomes_to_pick; i++)
		{
			std::vector<Genome> local_tournament;
			uint32_t number_to_select = std::min(window_size, (uint32_t)population_copy.size() - 1 - index);

			if (number_to_select == 0)
			{
				break;
			}

			for (uint32_t j = index; j < index + number_to_select; j++)
			{
				local_tournament.push_back(population_copy[j]);
			}

			auto max_it = std::max_element(local_tournament.begin(), local_tournament.end(),
				[](const Genome& a, const Genome& b)
				{
					return a.getFitness() < b.getFitness();
				});

			parents.push_back(local_tournament[max_it - local_tournament.begin()]);
			index += number_to_select;

		}
	}
	else
	{
		// With replacement
		std::uniform_int_distribution<unsigned int> distribution(0, int(population_copy.size() - 1));

		std::set<int> used_indices;
		std::set<int> selected_indices;
		std::vector<Genome> local_tournament;

		for (uint32_t i = 0; i < genomes_to_pick; i++)
		{

			local_tournament.clear();
			selected_indices.clear();
			while (selected_indices.size() != window_size)
			{
				if (selected_indices.size() + used_indices.size() == population_copy.size())
				{
					break;
				}

				int index = distribution(gen_mt);
				if (used_indices.find(index) == used_indices.end())
				{
					selected_indices.insert(index);
				}
			}
			// Convert Selected set into vector
			std::vector<int> selected_indices_v(selected_indices.begin(), selected_indices.end());

			for (uint32_t j = 0; j < selected_indices_v.size(); j++)
			{
				local_tournament.push_back(population_copy[selected_indices_v[j]]);
			}

			auto max_it = std::max_element(local_tournament.begin(), local_tournament.end(),
				[](const Genome& a, const Genome& b)
				{
					return a.getFitness() < b.getFitness();
				});

			parents.push_back(local_tournament[max_it - local_tournament.begin()]);
			used_indices.insert(selected_indices_v[max_it - local_tournament.begin()]);

		}
	}

	return parents;
}

void GAEncoding_Ass1::survivorSelection(int policy, int survivorSize)
{
	if (policy == 0)
	{
		m_offspring.insert(m_offspring.end(), m_elite.begin(), m_elite.end());
		m_population = uFromGammaPolicy(survivorSize);
		m_offspring.clear();
	}
	else if (policy == 1)
	{
		m_offspring.insert(m_offspring.end(), m_elite.begin(), m_elite.end());
		m_population = uPlusGammaPolicy(survivorSize);
		m_offspring.clear();
	}
	else if (policy == 2)
	{

		// For uFromGammaPolicy_Fuds, since high fitness individuals can be randomly
		// eliminated, we add the elites to the population AFTER the selection.
		m_population = uFromGammaPolicy_FUDS(survivorSize);
		m_population.insert(m_population.end(), m_elite.begin(), m_elite.end());
		m_offspring.clear();
	}
}

std::vector<Genome> GAEncoding_Ass1::uFromGammaPolicy(int survivorSize) 
{
	if (survivorSize == 0)
	{
		survivorSize = starting_pop_size;
	}

	// Sort the children based on decreasing fitness
	std::sort(m_offspring.begin(), m_offspring.end(), [](const Genome& lhs, const Genome& rhs) {return lhs.getFitness() > rhs.getFitness(); });

	// Choose the top u survivors
	if (survivorSize >= m_offspring.size())
	{
		return m_offspring;
	}

	std::vector<Genome>::const_iterator first = m_offspring.begin();
	std::vector<Genome>::const_iterator last = m_offspring.begin() + survivorSize;
	std::vector<Genome> survivors(first, last);

	return survivors;
}

std::vector<Genome> GAEncoding_Ass1::uFromGammaPolicy_FUDS(int survivorSize)
{
	// Initialize the bin
	std::map<int, std::pair<int, std::vector<int>>> bins;

	// Start building the bin
	for (int i = 0; i < m_offspring.size(); i++)
	{
		auto elem = bins.find(m_offspring[i].getFitness());
		if (elem == bins.end())
		{
			// First is the number of elements with that fitness, second is all their indices
			bins[m_offspring[i].getFitness()] = std::make_pair(1, std::vector<int>{i}); 
			continue;
		}

		bins[m_offspring[i].getFitness()].first++;
		bins[m_offspring[i].getFitness()].second.push_back(i);
	}

	// Now that the bin is built, we create our probability map using the pairs in bins
	int runningtotal = 0; // Mostly for debug purposes
	std::multimap<int, std::pair<int, std::vector<int>>> probabilities;

	for (auto const& [fitness, indices] : bins)
	{
		// indices.first is frequency, indices.second is indices
		probabilities.insert({ indices.first, {fitness, indices.second} });
		runningtotal += indices.first;
	}

	// Get boundaries and create random distribution
	int maxprob = probabilities.rbegin()->first;
	int minprob = probabilities.begin()->first;
	int range = maxprob - minprob;

	// Now select the indices to go extinct and not make it past survivor selection
	std::vector<float> probFloats = generateRandVecFloat((int)m_offspring.size() - survivorSize, gen_mt);
	//std::vector<int> extinctionIndices(m_offspring.size() - survivorSize);
	std::set<int> extinctionIndices;

	std::multimap<int, std::pair<int, std::vector<int>>>::iterator toFind_it;
	//auto toFind_it;
	for (float f : probFloats)
	{
		int toFind = (int)((f * (maxprob - minprob)) + minprob);

		// Check if toFind is the same as maxprob since it won't work the intended way.
		if (toFind == maxprob)
		{
			toFind_it = probabilities.lower_bound(toFind);	
		}
		else {

			// Remove the extracted index
			toFind_it = probabilities.upper_bound(toFind);
		}

		// Pop an index from the vector stored at the iterator
		extinctionIndices.insert(toFind_it->second.second.back());
		
		// Check if that was the last element in the vector
		if (toFind_it->second.second.size() == 1)
		{
			// Remove that element from the map
			toFind_it->second.second.pop_back();
			probabilities.erase(toFind_it);

			// Recalculate maxprob and minprob
			maxprob = probabilities.rbegin()->first;
			minprob = probabilities.begin()->first;
		}
		else {
			toFind_it->second.second.pop_back();
		}
		// Should the prob be decremented once an index is removed?
	}

	// Now that we have all our indices, copy those elements over from the offspring to the population
	std::vector<Genome> survivors;

	for (int i = 0; i < m_offspring.size(); i++)
	{
		if (extinctionIndices.find(i) == extinctionIndices.end())
		{
			survivors.push_back(m_offspring[i]);
		}
	}

	return survivors;

}

std::vector<Genome> GAEncoding_Ass1::uPlusGammaPolicy(int survivorSize)
{
	if (survivorSize == 0)
	{
		survivorSize = starting_pop_size;
	}

	// Combine the two vectors
	std::vector<Genome> newpop;
	newpop.reserve(m_parents.size() + m_offspring.size());
	newpop.insert(newpop.end(), m_parents.begin(), m_parents.end());
	newpop.insert(newpop.end(), m_offspring.begin(), m_offspring.end());

	// Sort
	std::sort(newpop.begin(), newpop.end(), [](const Genome& lhs, const Genome& rhs) {return lhs.getFitness() > rhs.getFitness(); });

	// Choose the top u survivors
	if (survivorSize >= newpop.size())
	{
		return newpop;
	}

	std::vector<Genome>::const_iterator first = newpop.begin();
	std::vector<Genome>::const_iterator last = newpop.begin() + survivorSize;
	std::vector<Genome> survivors(first, last);

	return survivors;

}

void GAEncoding_Ass1::permutationRandomSwap(Genome& gen, const uint32_t num_of_swaps)
{
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));

	// Pick from distribution
	auto pos1 = distribution(gen_mt);
	auto pos2 = distribution(gen_mt);
	
	// Keep cycling until pos1 and pos2 are different
	while (pos1 == pos2)
	{
		pos2 = distribution(gen_mt);
	}
	
	// Perform an iterswap between the two positions
	std::iter_swap(gen.genome_encoding_2b2_int.begin() + pos1, gen.genome_encoding_2b2_int.begin() + pos2);
	gen.setFitness(fitnessOfGenome(gen));
	
}

void GAEncoding_Ass1::permutationSwap(Genome& gen, const uint32_t pos1, const uint32_t pos2)
{
	// Check pos1 and pos2 are not same
	if (pos1 == pos2)
	{
		return;
	}

	std::iter_swap(gen.genome_encoding_2b2_int.begin() + pos1, gen.genome_encoding_2b2_int.begin() + pos2);
}

void GAEncoding_Ass1::permutationInsert(Genome& gen, const uint32_t initial_pos, const uint32_t final_pos)
{
	// Check pos1 and pos2 are not same
	if (initial_pos == final_pos)
	{
		return;
	}

	unsigned int offset = 0;
	if (initial_pos > final_pos)
	{
		offset = 1;
	}

	// Get vector from genome
	std::vector<std::vector<int>> genome_enc = gen.genome_encoding_2b2_int;

	// Store the initial position data
	std::vector<int> initial = gen.genome_encoding_2b2_int[initial_pos];

	// Insert the initial vect into final position
	gen.genome_encoding_2b2_int.insert(gen.genome_encoding_2b2_int.begin() + final_pos, initial);

	// Erase the original entry
	gen.genome_encoding_2b2_int.erase(gen.genome_encoding_2b2_int.begin() + initial_pos + offset);
}

void GAEncoding_Ass1::permutationRandomInsert(Genome& gen)
{
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));

	// Pick from distribution
	unsigned int initial_pos = distribution(gen_mt);
	unsigned int final_pos;

	do {
		final_pos = distribution(gen_mt);
	} while (initial_pos == final_pos); // Check pos1 and pos2 are same

	unsigned int offset = 0;
	if (initial_pos > final_pos)
	{
		offset = 1;
	}

	// Get vector from genome
	std::vector<std::vector<int>> genome_enc = gen.genome_encoding_2b2_int;

	// Store the initial position data
	std::vector<int> initial = gen.genome_encoding_2b2_int[initial_pos];

	// Insert the initial vect into final position
	gen.genome_encoding_2b2_int.insert(gen.genome_encoding_2b2_int.begin() + final_pos, initial);

	// Erase the original entry
	gen.genome_encoding_2b2_int.erase(gen.genome_encoding_2b2_int.begin() + initial_pos + offset);

	gen.setFitness(fitnessOfGenome(gen));
}

void GAEncoding_Ass1::permutationScramble(Genome& gen, std::vector<int> indices)
{
	std::vector<int> shuffled_indices = shuffleVector(indices);

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		permutationSwap(gen, indices[i], shuffled_indices[i]);
	}
}

void GAEncoding_Ass1::permutationRandomScramble(Genome& gen, float mutation_ratio)
{
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));

	// Pick from distribution
	unsigned int size_of_indices = (unsigned int)(mutation_ratio * gen.genome_encoding_2b2_int.size());
	if (size_of_indices == 0)
	{
		return;
	}

	std::set<int> shuffled_indices;
	while (shuffled_indices.size() != size_of_indices)
	{
		shuffled_indices.insert(distribution(gen_mt));
	}

	std::vector<int> indices(shuffled_indices.begin(), shuffled_indices.end());
	std::vector<int> shuffled_indices_v(shuffled_indices.begin(), shuffled_indices.end());
	std::sort(indices.begin(), indices.end());
	shuffled_indices_v = shuffleVector(shuffled_indices_v);

	unsigned int counter = 0;
	for (auto& n : shuffled_indices_v)
	{
		permutationSwap(gen, indices[counter], n);
		counter++;
	}
	gen.setFitness(fitnessOfGenome(gen));
}

void GAEncoding_Ass1::permutationInvert(Genome& gen, std::vector<int> indices)
{
	std::sort(indices.begin(), indices.end());
	std::vector<int> shuffled_indices = indices;
	std::sort(shuffled_indices.begin(), shuffled_indices.end(), std::greater<int>());

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		permutationSwap(gen, indices[i], shuffled_indices[i]);
	}
}

void GAEncoding_Ass1::permutationRandomInvert(Genome& gen, float mutation_ratio)
{
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));

	// Pick from distribution
	unsigned int size_of_indices = (unsigned int)(mutation_ratio * gen.genome_encoding_2b2_int.size());
	if (size_of_indices == 0)
	{
		return;
	}

	std::set<int> shuffled_indices_set;
	while (shuffled_indices_set.size() != size_of_indices)
	{
		shuffled_indices_set.insert(distribution(gen_mt));
	}

	std::vector<int> indices(shuffled_indices_set.begin(), shuffled_indices_set.end());

	std::sort(indices.begin(), indices.end());
	std::vector<int> shuffled_indices = indices;
	std::sort(shuffled_indices.begin(), shuffled_indices.end(), std::greater<int>());

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		permutationSwap(gen, indices[i], shuffled_indices[i]);
	}

	gen.setFitness(fitnessOfGenome(gen));
}

void GAEncoding_Ass1::permutationPointMutation(Genome& gen, unsigned int pos, unsigned int rot)
{
	if (rot > 3) rot = 3;
	gen.genome_encoding_2b2_int[pos][1] = rot;
}

void GAEncoding_Ass1::permutationRandomPointMutation(Genome& gen, float mutation_ratio)
{
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));

	// Pick from distribution
	unsigned int size_of_indices = (unsigned int)(mutation_ratio * gen.genome_encoding_2b2_int.size());
	if (size_of_indices == 0)
	{
		return;
	}

	std::set<int> shuffled_indices_set;
	while (shuffled_indices_set.size() != size_of_indices)
	{
		shuffled_indices_set.insert(distribution(gen_mt));
	}

	std::vector<int> indices(shuffled_indices_set.begin(), shuffled_indices_set.end());

	std::sort(indices.begin(), indices.end());
	std::vector<int> shuffled_indices = indices;
	std::sort(shuffled_indices.begin(), shuffled_indices.end(), std::greater<int>());

	std::uniform_int_distribution<unsigned int> distribution_rot(1, 3);
	unsigned int rot; 
	for (unsigned int i = 0; i < indices.size(); i++)
	{
		rot = distribution_rot(gen_mt);
		permutationPointMutation(gen, indices[i], rot);
	}

	gen.setFitness(fitnessOfGenome(gen));
}

void GAEncoding_Ass1::permutationRandomPointMutationOriginal(Genome& gen)
{
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));
	std::uniform_int_distribution<unsigned int> distribution_rot(0, 3);

	// Pick from distribution
	unsigned int index = distribution(gen_mt);
	unsigned int rot = distribution_rot(gen_mt);

	gen.genome_encoding_2b2_int[index][1] = rot; // Randomize the rotation
	gen.setFitness(fitnessOfGenome(gen));
}

void GAEncoding_Ass1::permutationRandomInvertOriginal(Genome& gen)
{
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));

	// Pick from distribution
	unsigned int size_of_indices = distribution(gen_mt);

	std::set<int> shuffled_indices_set;
	while (shuffled_indices_set.size() != size_of_indices)
	{
		shuffled_indices_set.insert(distribution(gen_mt));
	}

	std::vector<int> indices(shuffled_indices_set.begin(), shuffled_indices_set.end());

	std::sort(indices.begin(), indices.end());
	std::vector<int> shuffled_indices = indices;
	std::sort(shuffled_indices.begin(), shuffled_indices.end(), std::greater<int>());

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		permutationSwap(gen, indices[i], shuffled_indices[i]);
	}

	gen.setFitness(fitnessOfGenome(gen));
}

void GAEncoding_Ass1::permutationRandomScrambleOriginal(Genome& gen)
{
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));

	// Pick from distribution
	unsigned int size_of_indices = distribution(gen_mt);

	std::set<int> shuffled_indices;
	while (shuffled_indices.size() != size_of_indices)
	{
		shuffled_indices.insert(distribution(gen_mt));
	}

	std::vector<int> indices(shuffled_indices.begin(), shuffled_indices.end());
	std::vector<int> shuffled_indices_v(shuffled_indices.begin(), shuffled_indices.end());
	std::sort(indices.begin(), indices.end());
	shuffled_indices_v = shuffleVector(shuffled_indices_v);

	unsigned int counter = 0;
	for (auto& n : shuffled_indices_v)
	{
		permutationSwap(gen, indices[counter], n);
		counter++;
	}
	gen.setFitness(fitnessOfGenome(gen));
}

void GAEncoding_Ass1::permutationRandomSlide(Genome& gen)
{
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, 3);
	std::uniform_int_distribution<unsigned int> distributionBool(0, 1);

	// Pick from distribution
	unsigned int bounding_box_row = distribution(gen_mt);
	unsigned int bounding_box_col = distribution(gen_mt);
	unsigned int slide = distribution(gen_mt);
	bool vertical = distributionBool(gen_mt);

	std::vector<std::vector<int>> indices;
	std::vector<int> index = {0,0};
	indices.push_back(index);
	index[0] = bounding_box_row;
	index[1] = bounding_box_col;
	indices.push_back(index);
	
	// Convert Genome to 2D array
	std::vector<std::vector<std::vector<int>>> vect_2d = vectTo2D_g(gen.genome_encoding_2b2_int, HEIGHT, WIDTH);

	// Perform the slide
	vect_2d = vectSlide_g(vect_2d, indices, slide, vertical);
	
	// Convert back to 1 Dimensional
	gen.genome_encoding_2b2_int = vectTo1D_g(vect_2d, HEIGHT, WIDTH);
	gen.setFitness(fitnessOfGenome(gen));
}

void GAEncoding_Ass1::permutationSlide(Genome& gen)
{
	// Get a random bounding box in a genome
	std::vector<unsigned int> bbox1 = getBoundingBox(WIDTH, HEIGHT, gen_mt);

	// Get bounding box dimensions
	unsigned int bbrows = bbox1.back() / WIDTH - bbox1.front() / WIDTH + 1;
	unsigned int bbcols = (bbox1.back() % WIDTH - bbox1.front() % WIDTH) + 1;

	int x1 = bbox1.front() % WIDTH;
	int y1 = bbox1.front() / WIDTH;

	int x2 = bbox1.back() % WIDTH;
	int y2 = bbox1.back() / WIDTH;

	int slide_x;
	int slide_y;

	std::uniform_int_distribution<> direction(0, 1);

	// Generate a y slide
	int dir_y = direction(gen_mt); // 0 is up 1 is down

	if ((dir_y == 0) && (y1 > 0)) {
		std::uniform_int_distribution<> translation(0, y1);
		slide_y = -1 * translation(gen_mt);
	}
	else if ((dir_y == 1) && (y2 < HEIGHT))
	{
		std::uniform_int_distribution<> translation(y2, HEIGHT);
		slide_y = translation(gen_mt);
	}
	else {
		slide_y = 0;
	}

	// Generate an x slide
	int dir_x = direction(gen_mt); // 0 is left 1 is right
	if ((dir_x == 0) && (x1 > 0)) {
		std::uniform_int_distribution<> translation(0, x1);
		slide_x = -1 * translation(gen_mt);
	}
	else if ((dir_x == 1) && (x2 < HEIGHT))
	{
		std::uniform_int_distribution<> translation(x2, HEIGHT);
		slide_x = translation(gen_mt);
	}
	else {
		slide_x = 0;
	}

	// Apply slides
	std::vector<unsigned int> bbox2;
	for (int i = 0; i < bbox1.size(); i++)
	{
		bbox2.push_back(bbox1[i] + slide_x + slide_y * WIDTH);
	}

	std::map<int, int> map_1_2;
	std::map<int, int> map_left_over;

	// Build the first map
	for (int i = 0; i < bbox1.size(); i++)
	{
		map_1_2[bbox1[i]] = bbox2[i];
	}

	// Build the map for overlapping indices
	std::map<int, int> copy_1_2(map_1_2);
	for (auto const& x : map_1_2)
	{
		if (!copy_1_2.empty() && (copy_1_2.find(x.second) != copy_1_2.end()))
		{

			map_left_over[x.first] = copy_1_2[x.second];
			copy_1_2.erase(copy_1_2.find(x.first));
		}
	}

	// Perform swaps in copy_1_2 first
	for (auto const& x : copy_1_2)
	{
		permutationSwap(gen, x.first, x.second);
	}

	// Perform the swaps in map_left_over

	for (auto const& x : map_left_over)
	{
		permutationSwap(gen, x.first, x.second);
	}

}

void GAEncoding_Ass1::injectParents(float proportion)
{
	int num_new_parents = (int)(m_parents.size() * proportion);

	for (int i = 0; i < num_new_parents; i++)
	{
		m_parents.push_back(returnRandomlyInitializedGenome());
	}
}

std::string GAEncoding_Ass1::genotypeToPhenotype(Genome g)
{
	std::stringstream phenotype;

	// Cycle through all the tiles
	for (int i = 0; i < g.genome_encoding_2b2_int.size(); i++)
	{
		// Get the tile
		Tile t = m_map_index[g.genome_encoding_2b2_int[i][0]];

		// Apply the rotations
		t.rotate(g.genome_encoding_2b2_int[i][1]);

		// Add to string stream
		phenotype << t.top << t.right << t.bottom << t.left;

		// if we reached the end of a row, add a newline. Else, add a space
		if (i == g.genome_encoding_2b2_int.size() - 1)
		{
			break;
		}

		if (((i + 1) % WIDTH == 0))
		{
			phenotype << std::endl;
		}
		else {
			phenotype << " ";
		}

	}

	return phenotype.str();
}

void GAEncoding_Ass1::permutationRandomDiversify(std::vector<Genome>& gen_v, const float purge_ratio)
{
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen_v.size() - 1));

	// Pick from distribution
	unsigned int number_of_parents_to_pick = distribution(gen_mt);
	std::set<unsigned int> selected_indices;

	while (selected_indices.size() != (int)(purge_ratio * gen_v.size()))
	{
		int index = distribution(gen_mt);
		selected_indices.insert(index);
	}

	// Convert the set to a vector
	//std::vector<int> selected_indices_v(selected_indices.begin(), selected_indices.end());

	for (auto index : selected_indices)
	{
		gen_v[index] = returnRandomlyInitializedGenome();
	}
}

Genome GAEncoding_Ass1::returnRandomlyInitializedGenome()
{
	Genome tmp_gen;
	std::vector<std::vector<int>> randomized_genome = shuffleVector(m_original_genome);
	// Add random rotations
	std::uniform_int_distribution<unsigned int> distribution_rot(0, 3);
	for (unsigned int i = 0; i < randomized_genome.size(); i++)
	{
		randomized_genome[i][1] = distribution_rot(gen_mt);
	}
	tmp_gen.genome_encoding_2b2_int = randomized_genome;
	tmp_gen.setFitness(fitnessOfGenome(tmp_gen));
	return tmp_gen;
}

void GAEncoding_Ass1::recombination(float crossoverProb, int goalOffspringSize, bool skipCrossover)
{

	if (skipCrossover)
	{
		m_offspring = m_parents;
	}

	// Add the elites back to the parent pool so that they have a chance to reproduce
	for (auto elite : m_elite)
	{
		if (!(std::find(m_parents.begin(), m_parents.end(), elite) != m_parents.end()))
		{
			m_parents.push_back(elite);
		}
	}

	int breedingsize = (int)(m_parents.size() * crossoverProb);
	std::uniform_int_distribution distr(0, (int)(breedingsize));
	std::vector<Genome> pair, babies;

	if (breedingsize < 2)
	{
		genetic_algo_log() << "Trying to enter recombination with less than 2 parents. No offspring created.";
		return;
	}

	std::vector<unsigned int> bounding_box;
	while (m_offspring.size() < goalOffspringSize)
	{
		int mating_parent = 0;
		for (int i = 0; i < breedingsize; i++)		// Loop through parents designated for mating
		{
			pair.push_back(m_parents[i]);
			while (mating_parent == i)
			{
				mating_parent = distr(gen_mt);		// Select another parent that isn't the same parent
			} 
			pair.push_back(m_parents[mating_parent]);

			bounding_box = getBoundingBox(WIDTH, HEIGHT, gen_mt);
			babies = partiallyMappedCrossover(pair[0], pair[1], bounding_box);		// Crossover
			m_offspring.insert(m_offspring.end(), babies.begin(), babies.end());	// Store in offspring
			pair.clear();															// Reset

			if (m_offspring.size() >= goalOffspringSize)							// Break if desired size achieved
			{
				break;
			}
		}
		
	}

}

void GAEncoding_Ass1::mutation(float mutationProb, bool accelerated, bool slide)
{
	// Applies a random mutation to random offspring with a mutation probability mutationProb

	
	unsigned int permuatation_val = 1;
	// Generate distribution
	if (slide)
	{
		permuatation_val = 2;
	}

	std::uniform_int_distribution<unsigned int> distribution(0, permuatation_val);

	// Pick from distribution to select mutation to use
	unsigned int mutation_to_use; 

	for (unsigned int i = 0; i < m_offspring.size(); i++)
	{
		mutation_to_use = distribution(gen_mt);

		if (mutation_to_use == 0)
		{
			if (accelerated)
			{
				permutationRandomScrambleOriginal(m_offspring[i]);
			}
			else
			{
				permutationRandomScramble(m_offspring[i], mutationProb);
			}
		}
		else if (mutation_to_use == 1)
		{
			if (accelerated)
			{
				permutationRandomPointMutationOriginal(m_offspring[i]);
			}
			else
			{
				permutationRandomPointMutation(m_offspring[i], mutationProb);
			}
		}
		else if (mutation_to_use == 2)
		{
			permutationRandomSlide(m_offspring[i]);
		}
	}

}

std::vector<Genome> GAEncoding_Ass1::singlePointCrossover(Genome& parent1, Genome& parent2) // Not used because we need PMX
{
	std::vector<std::vector<int>> off1vec(parent1.genome_encoding_2b2_int), off2vec(parent2.genome_encoding_2b2_int);

	// Get random integer between start + 1 and end - 1
	std::uniform_int_distribution<> distr(1, (int)(parent1.getSize() - 1));
	int splitPoint = distr(gen_mt);

	// Swap the vectors
	std::swap_ranges(off1vec.begin(), off1vec.begin() + splitPoint, off2vec.begin());

	// Set the fitness of the resulting offspring
	Genome off1(off1vec);
	Genome off2(off2vec);
	off1.setFitness(fitnessOfGenome(off1));
	off2.setFitness(fitnessOfGenome(off2));

	return std::vector<Genome> {off1, off2};
}

std::vector<Genome> GAEncoding_Ass1::partiallyMappedCrossover(Genome& parent1, Genome& parent2, std::vector<unsigned int> cross_indices)
{
	std::vector<Genome> output;
	if (cross_indices.size() == 0)
	{
		return output;
	}

	std::sort(cross_indices.begin(), cross_indices.end());	// Sort them in increasing order

	Genome child1;
	Genome child2;
	child1.genome_encoding_2b2_int = parent2.genome_encoding_2b2_int;
	child2.genome_encoding_2b2_int = parent1.genome_encoding_2b2_int;

	// These maps are used to store the indices of each value for the vectors
	// For quick querry
	std::map<int, int> map_parent1;
	std::map<int, int> map_parent2;

	// Initialize the maps
	for (unsigned int i = 0; i < parent1.genome_encoding_2b2_int.size(); i++)
	{
		map_parent1[parent1.genome_encoding_2b2_int[i][0]] = i;
		map_parent2[parent2.genome_encoding_2b2_int[i][0]] = i;
	}

	// We'll loop through the mapped intersecton each entry at a time
	// For example, if 4 is being mapped to the 8 spot, we'll move the 8
	// to the 4 spot because there is currently a hole there. If we keep doing
	// these swaps down the crossed section, all the pieces will fall into place
	for (unsigned int i = 0; i < cross_indices.size(); i++)
	{
		permutationSwap(child1, cross_indices[0], map_parent1[parent2.genome_encoding_2b2_int[cross_indices[0]][0]]);
		permutationSwap(child2, cross_indices[0], map_parent2[parent1.genome_encoding_2b2_int[cross_indices[0]][0]]);
	}

	child1.setFitness(fitnessOfGenome(child1));
	child2.setFitness(fitnessOfGenome(child2));

	output.push_back(child1);
	output.push_back(child2);

	return output;
}

Genome GAEncoding_Ass1::getGenomeFromPopulation(const unsigned int gen_num)
{
	return m_population[gen_num];
}

float GAEncoding_Ass1::getAverageFitness(std::vector<Genome> population)
{
	float average = 0;

	for (auto genome : population)
	{
		average += genome.getFitness();
	}
	return average / (float)population.size();
}

bool GAEncoding_Ass1::terminationConditions(int currentGen, int maxGeneration, double currRuntime, double maxRuntime, int targetFitness)
{
	if (maxGeneration != -1 && (currentGen == maxGeneration))
	{
		return true;
	}

	if (maxRuntime < 0 && (currRuntime >= maxRuntime))
	{
		return true;
	}

	return false;

}

std::string GAEncoding_Ass1::savePopulation()
{

	std::string date = getCurrentDateTime("date");
	std::ofstream fout;
	std::string file_name = "population_" + date + ".txt";
	fout.open(file_name);

	for (unsigned int i = 0; i < m_population.size(); i++)
	{

		fout << "FITNESS = " << std::to_string(m_population[i].getFitness())
			<< ": ROW_MISMATCHES = " << std::to_string(m_population[i].getRowMismatches())
			<< ": COL_MISMATCHES = " << std::to_string(m_population[i].getColMismatches()) << "|";

		for (unsigned int j = 0; j < m_population[i].genome_encoding_2b2_int.size(); j++)
		{
			fout << m_population[i].genome_encoding_2b2_int[j][0] << "," << m_population[i].genome_encoding_2b2_int[j][1] << ";";
		}
		fout << "\n";
	}

	return file_name;
}

void GAEncoding_Ass1::loadPopulation(std::string file_name, unsigned int starting_pop_size)
{
	m_population.clear();

	std::ifstream fin(file_name);
	std::string line;
	std::string delimiter = ";";

	if (fin.is_open())
	{
		while (std::getline(fin, line))
		{
			Genome genome;
			size_t pos = 0;
			std::string token;
			std::vector<std::string> gene;
			std::vector<int> couple;

			if (line.find("|") != std::string::npos)
			{
				std::vector<std::string> data;
				data = splitString(line, "|");
				line = data[1];
			}

			while ((pos = line.find(delimiter)) != std::string::npos) 
			{
				token = line.substr(0, pos);
				gene = splitString(token, ",");
				line.erase(0, pos + delimiter.length());
				couple.push_back(std::stoi(gene[0]));
				couple.push_back(std::stoi(gene[1]));
				genome.genome_encoding_2b2_int.push_back(couple);
				couple.clear();
				gene.clear();
			}

			genome.setFitness(fitnessOfGenome(genome));
			m_population.push_back(genome);
			genome.genome_encoding_2b2_int.clear();

			if (m_population.size() == starting_pop_size)
			{
				break;
			}
		}
		fin.close();
	}

	// Set the population size attribute of the encoding
	unsigned int remaining_genomes = starting_pop_size - (int)m_population.size();
	if (remaining_genomes > 0)
	{
		for (unsigned int i = 0; i < remaining_genomes; i++)
		{
			m_population.push_back(returnRandomlyInitializedGenome());
		}
	}

}

Genome GAEncoding_Ass1::getEliteFromFile(std::string file_name)
{

	std::vector<Genome> loaded_pop;

	std::ifstream fin(file_name);
	std::string line;
	std::string delimiter = ";";

	if (fin.is_open())
	{
		while (std::getline(fin, line))
		{
			Genome genome;
			size_t pos = 0;
			std::string token;
			std::vector<std::string> gene;
			std::vector<int> couple;

			if (line.find("|") != std::string::npos)
			{
				std::vector<std::string> data;
				data = splitString(line, "|");
				line = data[1];
			}

			while ((pos = line.find(delimiter)) != std::string::npos)
			{
				token = line.substr(0, pos);
				gene = splitString(token, ",");
				line.erase(0, pos + delimiter.length());
				couple.push_back(std::stoi(gene[0]));
				couple.push_back(std::stoi(gene[1]));
				genome.genome_encoding_2b2_int.push_back(couple);
				couple.clear();
				gene.clear();
			}

			genome.setFitness(fitnessOfGenome(genome));
			loaded_pop.push_back(genome);
			genome.genome_encoding_2b2_int.clear();
		}
		fin.close();
	}

	// Sort the population
	std::sort(loaded_pop.begin(), loaded_pop.end(), [](const Genome& lhs, const Genome& rhs) {return lhs.getFitness() > rhs.getFitness(); });

	return loaded_pop[0];
}