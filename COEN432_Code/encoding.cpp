
#include "encoding.h"


size_t Genome::getSize()
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

int GAEncoding_Ass1::fitnessOfGenome(std::vector<std::vector<int>> genome)
{
	// 8x8 box - meaning 

	int mismatched = 0;

	// Calculate rows
	for (unsigned int i = 0; i < genome.size() - 1; i++)
	{
		if (i % WIDTH == 0) 
		{
			continue;
		}
		
		if (m_map_index[genome[i][0]].right != m_map_index[genome[i + 1][0]].left)
		{
			mismatched++;
		}
	}

	// Calculate Columns
	for (unsigned int i = 0; i < genome.size() - WIDTH; i++)
	{

		if (m_map_index[genome[i][0]].bottom != m_map_index[genome[i + WIDTH][0]].top)
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

	for (unsigned int i = 0; i < number_of_genomes; i++)
	{
		Genome tmp_gen;
		std::vector<std::vector<int>> randomized_genome = shuffleVector(m_original_genome);
		tmp_gen.genome_encoding_2b2_int = randomized_genome;
		tmp_gen.fitness = fitnessOfGenome(randomized_genome);
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

	// 2. Sort according to fitness
	std::sort(m_population.begin(), m_population.end(), [](const Genome& lhs, const Genome& rhs){return lhs.fitness > rhs.fitness;});

	// 3. append the carry over to parents + remove them from original population list
	std::vector<Genome> parents;
	for (unsigned int i = 0; i < carry_over; i++)
	{
		parents.push_back(m_population[0]);			// Store the first element in parents
		m_population.erase(m_population.begin());		// Erase the first element in population
	}

	// 4. Call Fitness Proportionate if selected
	std::vector<Genome> parents2;
	if (strategy == 0)
	{
		parents2 = parentSelectionFitnessProportionate(m_population, selection_ratio);
	}
	// 4b. Call Tournament if selected
	else if (strategy == 1)
	{
		parents2 = parentSelectionTournament(m_population, selection_ratio, window_size, replacement);
	}
	
	// 5. Merge vectors together 
	m_parents.reserve(parents.size() + parents2.size()); // preallocate memory
	m_parents.insert(m_parents.end(), parents.begin(), parents.end());
	m_parents.insert(m_parents.end(), parents2.begin(), parents2.end());

	// Complete
}

std::vector<Genome> GAEncoding_Ass1::parentSelectionFitnessProportionate(std::vector<Genome> population, float selection_ratio)
{
	std::vector<Genome> parents;

	// Find total 
	unsigned int max = 0;
	for (unsigned int i = 0; i < population.size(); i++)
	{
		max += population[i].fitness;
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
			current += population[i].fitness;
			if (current > pick)
			{
				parents.push_back(population[i]);
				max -= population[i].fitness;
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

	std::vector<Genome> parents;

	unsigned int genomes_to_pick = (unsigned int)(selection_ratio * population.size());

	while (genomes_to_pick != 0)
	{
		if (population.size() == 0)
		{
			break;
		}

		std::uniform_int_distribution<unsigned int> distribution(0, int(population.size() - 1));

		// Select the indeces
		std::vector<size_t> indices;
		std::vector<Genome> local_tournament;
		uint32_t number_to_select = std::min(window_size, (uint32_t)population.size());
		for (unsigned int i = 0; i < number_to_select; i++)
		{
			unsigned int index;
			do {
				index = distribution(gen_mt);
			} 
			while (std::find(indices.begin(), indices.end(), index) != indices.end());
			indices.push_back(index);
			local_tournament.push_back(population[indices.back()]);
		}

		auto max_it = std::max_element(local_tournament.begin(), local_tournament.end(),
			[](const Genome& a, const Genome& b)
			{
				return a.fitness < b.fitness;
			});
		
		int value = max_it - local_tournament.begin();
		parents.push_back(population[indices[max_it - local_tournament.begin()]]);
		//indices.erase(indices.begin() + (max_it - local_tournament.begin()));
		

		if (!replacement)
		{
			population = erase_indices(population, indices);
		}
		else
		{
			population.erase(population.begin() + indices[max_it - local_tournament.begin()]);
		}
		
		genomes_to_pick--;
	}

	return parents;

}

void GAEncoding_Ass1::survivorSelection()
{

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
		std::cout << "Position 1 and Position 2 need to be different values." << std::endl;
		return;
	}

	std::iter_swap(gen.genome_encoding_2b2_int.begin() + pos1, gen.genome_encoding_2b2_int.begin() + pos2);
}

void GAEncoding_Ass1::permutationInsert(Genome& gen, const uint32_t initial_pos, const uint32_t final_pos)
{
	// Check pos1 and pos2 are not same
	if (initial_pos == final_pos)
	{
		std::cout << "Position 1 and Position 2 need to be different values." << std::endl;
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

void GAEncoding_Ass1::permutationScramble(Genome& gen, std::vector<int> indices)
{
	std::vector<int> shuffled_indices = shuffleVector(indices);

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		permutationSwap(gen, indices[i], shuffled_indices[i]);
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

void GAEncoding_Ass1::permutationPointMutation(Genome& gen, unsigned int pos)
{
	// Our point mutation will simply rotate the tile at the specified index
	// We should also consider rotating more than just once at a time.
	gen.genome_encoding_2b2_int[pos][1]++;
}

/**
* Acts on m_parents to create m_offspring
*/
void GAEncoding_Ass1::recombination(float crossoverProb)
{
	//

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
	std::uniform_int_distribution<> distr(1, parent1.getSize() - 1);
	int splitPoint = distr(gen_mt);

	// Swap the vectors
	std::swap_ranges(off1vec.begin(), off1vec.begin() + splitPoint, off2vec.end());

	return std::vector<Genome> {Genome(off1vec), Genome(off2vec)};

}

Genome GAEncoding_Ass1::getGenomeFromPopulation(const unsigned int gen_num)
{
	return m_population[gen_num];
}