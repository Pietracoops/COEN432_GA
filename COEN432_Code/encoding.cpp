
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

int GAEncoding_Ass1::fitnessOfGenome(const Genome& genome)
{
	// 8x8 box - meaning 

	int mismatched = 0;
	std::vector<std::vector<int>> genes = genome.getEncoding();

	// Calculate rows
	for (unsigned int i = 0; i < genome.getSize() - 1; i++)
	{
		if (i % WIDTH == 0) 
		{
			continue;
		}
		
		if (m_map_index[genes[i][0]].right != m_map_index[genes[i + 1][0]].left)
		{
			mismatched++;
		}
	}

	// Calculate Columns
	for (unsigned int i = 0; i < genome.getSize() - WIDTH; i++)
	{

		if (m_map_index[genes[i][0]].bottom != m_map_index[genes[i + WIDTH][0]].top)
		{
			mismatched++;
		}
	}

	return MAX_MISMATCHES - mismatched;
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
		Genome tmp_gen;
		std::vector<std::vector<int>> randomized_genome = shuffleVector(m_original_genome);
		tmp_gen.genome_encoding_2b2_int = randomized_genome;
		tmp_gen.setFitness(fitnessOfGenome(tmp_gen));
		m_population.push_back(tmp_gen);
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

}


GAEncoding_Ass1::~GAEncoding_Ass1()
{
	// Destructor
}

void GAEncoding_Ass1::parentSelection(int strategy, uint32_t carry_over, float selection_ratio, uint32_t window_size, bool replacement)
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

	// 3. append the carry over to parents + remove them from original population list
	for (unsigned int i = 0; i < carry_over; i++)
	{
		m_elite.push_back(m_population[0]);			// Store the first element in parents
		m_population.erase(m_population.begin());		// Erase the first element in population
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
	std::sort(m_offspring.begin(), m_offspring.end());

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
	std::sort(newpop.begin(), newpop.end());

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

}

void GAEncoding_Ass1::permutationScramble(Genome& gen, std::vector<int> indices)
{
	std::vector<int> shuffled_indices = shuffleVector(indices);

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		permutationSwap(gen, indices[i], shuffled_indices[i]);
	}
}

void GAEncoding_Ass1::permutationRandomScramble(Genome& gen)
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
	std::sort(indices.begin(), indices.end());

	unsigned int counter = 0;
	for (auto& n : shuffled_indices)
	{
		permutationSwap(gen, indices[counter], n);
		counter++;
	}
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

void GAEncoding_Ass1::permutationRandomInvert(Genome& gen)
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
}

void GAEncoding_Ass1::permutationPointMutation(Genome& gen, unsigned int pos)
{
	// Our point mutation will simply rotate the tile at the specified index
	// We should also consider rotating more than just once at a time.
	gen.genome_encoding_2b2_int[pos][1]++;
}

void GAEncoding_Ass1::permutationRandomPointMutation(Genome& gen)
{
	std::uniform_int_distribution<unsigned int> distribution(0, int(gen.genome_encoding_2b2_int.size() - 1));
	std::uniform_int_distribution<unsigned int> distribution_rot(1, 3);

	// Pick from distribution
	unsigned int index = distribution(gen_mt);
	unsigned int rot = distribution_rot(gen_mt);

	gen.genome_encoding_2b2_int[index][1] = rot; // Randomize the rotation
}

/**
* Acts on m_parents to create m_offspring.
* 
* TODO: add an argument that takes a crossover function so that it does not need to be changed manually
*/
void GAEncoding_Ass1::recombination(float crossoverProb, int goalOffspringSize, bool allowfailures)
{
	int parent1 = -1;
	int parent2 = -1;
	std::vector<Genome> babies;

	std::cout << "offspring size" << m_offspring.size() << std::endl;
	std::cout << "parent size" << m_parents.size() << std::endl;

	// Add the elites back to the parent pool so that they have a chance to reproduce
	m_parents.insert(m_parents.end(), m_elite.begin(), m_elite.end());

	while (m_offspring.size() < goalOffspringSize) {
		
		// Generate a vector of random floats
		std::vector<float> vec_randf = generateRandVecFloat((int)m_parents.size(), gen_mt);

		// Shuffle the parents
		m_parents = shuffleVector(m_parents);

		//Iterate through pairs of parents to create offspring
		for (int i = 0; i < vec_randf.size(); i++)
		{
			if (vec_randf[i] < crossoverProb)
			{
				if (parent1 == -1)
				{
					parent1 = i;
				}
				else if (parent2 == -1)
				{
					parent2 = i;

					// Call a crossover function
					babies = singlePointCrossover(m_parents[parent1], m_parents[parent2]);

					// Reset parent 1 and parent 2
					parent1 = -1;
					parent2 = -1;

					// Add the babies to the offspring pool
					m_offspring.insert(m_offspring.end(), babies.begin(), babies.end());
				}
				else {
					// This is an error case, something went wrong if were here
					// TODO: Add this error to the log
					std::cout << "Something went wrong in recombination.";
				}
			}
			else if (allowfailures) {

				// This parent does not reproduce and is instead added directly to the offspring pool
				if (!(std::find(m_elite.begin(), m_elite.end(), m_parents[i]) != m_elite.end()))
				{
					m_offspring.push_back(m_parents[i]);
				}				
			}
		}
	}

	std::cout << "final offspring size" << m_offspring.size() << std::endl;
}

void GAEncoding_Ass1::mutation(float mutationProb)
{
	// Applies a random mutation to random offspring with a mutation probability mutationProb

	int total_mutations = 5;

	// Generate a vector of random float probabilities
	std::vector<float> vec_randf = generateRandVecFloat((int)m_offspring.size(), gen_mt);
	std::uniform_real_distribution<> distr(0, 1);

	// Iterate through the offspring to mutate
	for (int i = 0; i < vec_randf.size(); i++)
	{
		// Corresponding individual will be mutated
		if (vec_randf[i] < mutationProb)
		{
			// Generate a random float between 0 and 1
			float prob = (float)distr(gen_mt);

			if (prob < 1.0 / total_mutations)
			{
				permutationRandomSwap(m_offspring[i], 1);
			}
			else if (prob < 2.0 / total_mutations)
			{
				permutationRandomInsert(m_offspring[i]);
			}
			else if (prob < 3.0 / total_mutations)
			{
				permutationRandomScramble(m_offspring[i]);
			}
			else if (prob < 4.0 / total_mutations)
			{
				permutationRandomInvert(m_offspring[i]);
			}
			else if (prob < 5.0 / total_mutations)
			{
				permutationRandomPointMutation(m_offspring[i]);
			}
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

std::vector<Genome> GAEncoding_Ass1::partiallyMappedCrossover(Genome& parent1, Genome& parent2)
{
	// Repeat this for each parent to generate 2 offspring

	//// 1. Choose two random cutting points for the segment
	//std::uniform_int_distribution<> distr1(0, parent1.getSize() - 1);
	//int index1 = distr1(gen_mt);
	//
	//while ((index1 == (parent1.getSize() - 1))) // Make sure index1 isn't the last element in the vector
	//{
	//	index1 = distr1(gen_mt);
	//}

	//std::uniform_int_distribution<> distr2(index1, parent1.getSize() - 1);
	//int index2 = distr2(gen_mt);

	//while (index1 == index2) // Make sure that the two indices aren't the same
	//{
	//	index2 = distr2(gen_mt);
	//}

	int index1 = 3;
	int index2 = 6;

	// 2. Copy over the segment

	// Into offspring 1
	std::vector<std::vector<int>> offspring1(parent1.getSize(), std::vector<int>(parent1.getEncoding()[0].size(), -1)); // initialize the tile values to -1
	std::copy(parent1.getEncoding().begin() + index1, parent1.getEncoding().begin() + index2,
		offspring1.begin() + index1);

	// Into offspring 2
	std::vector<std::vector<int>> offspring2(parent2.getSize(), std::vector<int>(parent2.getEncoding()[0].size(), -1)); // initialize the tile values to -1
	std::copy(parent2.getEncoding().begin() + index1, parent2.getEncoding().begin() + index2,
		offspring2.begin() + index1);

	std::unordered_map<int, int> p1map;
	std::unordered_map<int, int> p2map;
	std::unordered_map<int, int> off1map;
	std::unordered_map<int, int> off2map;
	// Create a map storing the locations of each of the tiles in the vectors
	// Inserting and finding values in a map are in O(1) time, so this means that
	// after 1 O(N) operation (the following loop), we do not have to keep searching
	// the parents for the locations of the corresponding values
	for (int i = 0; i < parent1.getSize(); i++)
	{
		// Key is the tile number, value is the location in the vector
		p1map[parent1.genome_encoding_2b2_int[i][0]] = i;
		p2map[parent2.genome_encoding_2b2_int[i][0]] = i;

		if (i >= index1 && i <= index2) {
			off1map[parent1.genome_encoding_2b2_int[i][0]] = i;
			off2map[parent2.genome_encoding_2b2_int[i][0]] = i;
		}
	}


	// Loop over the crossover points
	int value1, temp1, value2, temp2;
	int postempP1, postempP2;
	bool found1 = false; 
	bool found2 = false;
	for (int i = index1; i < index2; i++)
	{
		// ------- Building offspring1 ------- //
		value1 = parent2.getEncoding()[i][0];
		if ((off1map.find(value1) == off1map.end())) // If the value1 from p2 was not copied over
		{
			found1 = false;
			while (!found1)
			{
				temp1 = offspring1[off1map[value1]][0]; // element occupying the position in offspring 1
				postempP2 = p2map[temp1]; // corresponding index of the element in p2

				// Check if the corresponding position in offspring 1 is free
				if (offspring1[postempP2][0] == -1)
				{
					// Perform the swap
					offspring1.insert(offspring1.begin() + postempP2, parent2.getEncoding()[postempP2]);
					
					// Add the tile#, index pair to the off1map
					off1map[parent2.getEncoding()[postempP2][0]] = postempP2;

					found1 = true;
				}
			}
		}

		// ------- Building offspring2 ------- //
		value2 = parent1.getEncoding()[i][0];
		if ((off2map.find(value2) == off2map.end())) // If the value1 from p2 was not copied over
		{
			found2 = false;
			while (!found2)
			{
				temp2 = offspring2[off2map[value2]][0]; // element occupying the position in offspring 1
				postempP1 = p1map[temp2]; // corresponding index of the element in p2

				// Check if the corresponding position in offspring 1 is free
				if (offspring2[postempP1][0] == -1)
				{
					// Perform the swap
					offspring2.insert(offspring2.begin() + postempP1, parent2.getEncoding()[postempP1]);

					// Add the tile#, index pair to the off2map
					off2map[parent2.getEncoding()[postempP1][0]] = postempP1;

					found2 = true;
				}
			}
		}

	
	}

	// Fill in the rest of the elements that don't deal with the crossover segment
	for (int i = 0; i < offspring1.size(); i++)
	{
		if (offspring1[i][0] == -1)
		{
			offspring1[i] = parent2.getEncoding()[i];
		}

		if (offspring2[i][0] == -1)
		{
			offspring2[i] = parent1.getEncoding()[i];
		}
	}

	// Set the fitness of the resulting offspring
	Genome off1(offspring1);
	off1.setFitness(fitnessOfGenome(off1));

	Genome off2(offspring2);
	off2.setFitness(fitnessOfGenome(off2));

	return std::vector<Genome> {off1, off2};
}

Genome GAEncoding_Ass1::getGenomeFromPopulation(const unsigned int gen_num)
{
	return m_population[gen_num];
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
