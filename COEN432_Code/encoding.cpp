
#include "encoding.h"


size_t Genome::getSize() const
{
	return this->genome_encoding_2b2_int.size();
}


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
	if (rot_value > 3) rot_value = 3;

	std::vector<std::string> arr{ top, right, bottom, left };
	std::vector<std::string> tmp;

	for (unsigned int i = rot_value; i < arr.size(); i++)
	{
		tmp.push_back(arr[i]);
	}

	for (unsigned int i = 0; i < rot_value; i++)
	{
		tmp.push_back(arr[i]);
	}

	top		= tmp[0];
	right	= tmp[1];
	bottom	= tmp[2];
	left	= tmp[3];
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
	for (unsigned int i = 0; i < genome.getSize() - 1; i++)
	{
		if (i % WIDTH == 0) 
		{
			continue;
		}
		
		if (m_map_index[genes[i][0]].right != m_map_index[genes[i + 1][0]].left)
		{
			row_mismatches++;
		}
	}

	// Calculate Columns
	int col_mismatches = 0;
	for (unsigned int i = 0; i < genome.getSize() - WIDTH; i++)
	{

		if (m_map_index[genes[i][0]].bottom != m_map_index[genes[i + WIDTH][0]].top)
		{
			col_mismatches++;
		}
	}

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
	m_min_fitness = 0;
}


GAEncoding_Ass1::~GAEncoding_Ass1()
{
	// Destructor
}

void GAEncoding_Ass1::parentSelection(int strategy,
									  uint32_t carry_over,
									  float selection_ratio,
									  uint32_t window_size,
									  bool replacement,
									  float randomness,
									  float diversity_ratio,
									  float purge_ratio)
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

	// 3. append the carry over to parents + remove them from original population list
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
	unsigned int num_random_gen = (int)(parents.size() * randomness);
	// Parents is a shuffled array so we'll just pop out the last ones in array
	// and replace them with random initializations of the genome
	parents.erase(parents.end() - num_random_gen, parents.end());
	for (unsigned int i = 0; i < num_random_gen; i++)
	{
		parents.push_back(returnRandomlyInitializedGenome());
	}

	// If we are struggling to achieve desired diversity, soft reset
	if (((m_max_fitness - m_min_fitness) / m_max_fitness) < diversity_ratio)
	{
		permutationRandomDiversify(parents, purge_ratio);
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
		// Without replacement
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

/**
* 
*/
void GAEncoding_Ass1::survivorSelection(int policy, int survivorSize)
{
	if (policy == 0)
	{
		m_population = uFromGammaPolicy(survivorSize);
		m_offspring.clear();
	}
	else if (policy == 1)
	{
		m_population = uPlusGammaPolicy(survivorSize);
		m_offspring.clear();
	}
}

/**
* (u, g) policy. Pick u survivors from g children. Children are ranked based on fitness and u survivors are chosen
*/
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

/**
* Pick u survivors from offspring + parents
* 
* (Not recommended by prof and textbook)
*/
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
	std::sort(indices.begin(), indices.end());

	unsigned int counter = 0;
	for (auto& n : shuffled_indices)
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
	// Our point mutation will simply rotate the tile at the specified index
	// We should also consider rotating more than just once at a time.
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

/**
* Acts on m_parents to create m_offspring.
* 
* TODO: add an argument that takes a crossover function so that it does not need to be changed manually
*/
void GAEncoding_Ass1::recombination(float crossoverProb, int goalOffspringSize, bool skipCrossover)
{

	// Add the elites back to the parent pool so that they have a chance to reproduce
	for (auto elite : m_elite)
	{
		if (!(std::find(m_parents.begin(), m_parents.end(), elite) != m_parents.end()))
		{
			m_parents.push_back(elite);
		}
	}


	if (skipCrossover)
	{
		m_offspring = m_parents;
	}

	int breedingsize = m_parents.size() * crossoverProb;


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
		for (int i = 0; i < breedingsize; i++)								// Loop through parents designated for mating
		{
			pair.push_back(m_parents[i]);
			while (mating_parent != i)
			{
				mating_parent = distr(gen_mt);								// Select another parent that isn't the same parent
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

void GAEncoding_Ass1::mutation(float mutationProb)
{
	// Applies a random mutation to random offspring with a mutation probability mutationProb

	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, 2);

	// Pick from distribution to select mutation to use
	unsigned int mutation_to_use; 

	for (unsigned int i = 0; i < m_offspring.size(); i++)
	{
		mutation_to_use = distribution(gen_mt);

		if (mutation_to_use == 0)
		{
			permutationRandomScramble(m_offspring[i], mutationProb);
		}
		else if (mutation_to_use == 1)
		{
			permutationRandomInvert(m_offspring[i], mutationProb);
		}
		else if (mutation_to_use == 2)
		{
			permutationRandomPointMutation(m_offspring[i], mutationProb);
		}
	}

	// Add the elite to the offspring pool
	m_offspring.insert(m_offspring.end(), m_elite.begin(), m_elite.end());
}

/**
* Takes two parents and produces a vector containing 2 offspring genomes
* 
* THIS WILL NOT WORK FOR OUR PROBLEM, LEADS TO INADMISSIBLE SOLUTION
*/
std::vector<Genome> GAEncoding_Ass1::singlePointCrossover(Genome& parent1, Genome& parent2)
{
	std::vector<std::vector<int>> off1vec(parent1.genome_encoding_2b2_int), 
		off2vec(parent2.genome_encoding_2b2_int);

	// Get random integer between start + 1 and end - 1
	std::uniform_int_distribution<> distr(1, (int)(parent1.getSize() - 1));
	int splitPoint = distr(gen_mt);

	// Swap the vectors
	std::swap_ranges(off1vec.begin(), off1vec.begin() + splitPoint, off2vec.begin());

	// Set the fitness of the resulting offspring
	Genome off1(off1vec);
	off1.setFitness(fitnessOfGenome(off1));

	Genome off2(off2vec);
	off2.setFitness(fitnessOfGenome(off2));

	return std::vector<Genome> {off1, off2};

}

std::vector<Genome> GAEncoding_Ass1::partiallyMappedCrossover(Genome& parent1, Genome& parent2, std::vector<unsigned int> cross_indices)
{

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
	for (unsigned int i = cross_indices[0]; i <= cross_indices[1]; i++)
	{
		permutationSwap(child1, i, map_parent2[parent1.genome_encoding_2b2_int[i][0]]);
		permutationSwap(child2, i, map_parent1[parent2.genome_encoding_2b2_int[i][0]]);		
	}

	child1.setFitness(fitnessOfGenome(child1));
	child2.setFitness(fitnessOfGenome(child2));

	std::vector<Genome> output;
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

/**
* If arguments are not specified then the termination condition linked to those arguments
* will not execute.
*/
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


void GAEncoding_Ass1::savePopulation()
{

	std::string date = getCurrentDateTime("date");
	std::ofstream fout;
	fout.open("population_" + date + ".txt");

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

}

void GAEncoding_Ass1::loadPopulation(std::string file_name)
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
		}
		fin.close();
	}

}