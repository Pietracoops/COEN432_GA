
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

	return mismatched;
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

void GAEncoding_Ass1::parentSelection()
{

}

void GAEncoding_Ass1::parentSelectionFitnessProportionate(std::vector<Genome> population)
{

}

void GAEncoding_Ass1::parentSelectionTournament(std::vector<Genome> population, uint32_t window_size, bool replacement)
{

}

/**
* 
*/
void GAEncoding_Ass1::survivorSelection(int policy = 0, int survivorSize=0)
{
	if (policy == 0)
	{
		m_population = uFromGammaPolicy(survivorSize);
	}
	else if (policy == 1)
	{
		m_population = uPlusGammaPolicy(survivorSize);
	}
}

/**
* (u, g) policy. Pick u survivors from g children. Children are ranked based on fitness and u survivors are chosen
*/
std::vector<Genome> GAEncoding_Ass1::uFromGammaPolicy(int survivorSize=0) 
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
std::vector<Genome> GAEncoding_Ass1::uPlusGammaPolicy(int survivorSize=0)
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
	std::mt19937 mt(rd());

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
* Acts on m_parents to create m_offspring.
* 
* TODO: add an argument that takes a crossover function so that it does not need to be changed manually
*/
void GAEncoding_Ass1::recombination(float crossoverProb, bool allowfailures)
{
	int parent1 = -1;
	int parent2 = -1;
	std::vector<Genome> babies;

	// Generate a vector of random floats
	std::vector<float> vec_randf = generateRandVecFloat(m_parents.size(), gen_mt);

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
			m_offspring.push_back(m_parents[i]);
		}
	}
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

	// Set the fitness of the resulting offspring
	Genome off1(off1vec);
	off1.setFitness(fitnessOfGenome(off1));

	Genome off2(off2vec);
	off2.setFitness(fitnessOfGenome(off2));

	return std::vector<Genome> {off1, off2};

}

Genome GAEncoding_Ass1::getGenomeFromPopulation(const unsigned int gen_num)
{
	return m_population[gen_num];
}