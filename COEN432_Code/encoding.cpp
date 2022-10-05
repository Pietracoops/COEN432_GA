
#include "encoding.h"


void GAEncoding::initializaPopulation(const unsigned int number_of_genomes)
{

}

// Core Functions
void GAEncoding::parentSelection()
{
	std::cout << "parentSelection function not implemented." << std::endl;
}
void GAEncoding::recombination()
{
	std::cout << "recombination function not implemented." << std::endl;
}
void GAEncoding::survivorSelection()
{
	std::cout << "survivorSelection function not implemented." << std::endl;
}

void GAEncoding::permutationRandomSwap(Genome& gen, const uint32_t num_of_swaps)
{
	std::cout << "permutationRandomSwap function not implemented." << std::endl;
}

void GAEncoding::permutationSwap(Genome& gen, const uint32_t pos1, const uint32_t pos2)
{
	std::cout << "permutationSwap function not implemented." << std::endl;
}

Genome GAEncoding::getGenomeFromPopulation(const unsigned int gen_num)
{
	std::cout << "getGenomeFromPopulation function not implemented." << std::endl;
	Genome empty_genome;

	return empty_genome;
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

	for (unsigned int i = 0; i < number_of_genomes; i++)
	{
		Genome tmp_gen;
		std::vector<std::vector<int>> randomized_genome = shuffleVector(m_original_genome);
		tmp_gen.genome_encoding_2b2_int = randomized_genome;
		m_population.push_back(tmp_gen);
		m_population_fitness.push_back(fitnessOfGenome(randomized_genome));
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

void GAEncoding_Ass1::recombination()
{

}

void GAEncoding_Ass1::survivorSelection()
{

}

void GAEncoding_Ass1::permutationRandomSwap(Genome& gen, int num_of_swaps)
{
	// Get vector from genome
	std::vector<std::vector<int>> genome_enc = gen.genome_encoding_2b2_int;
	
	// Generate distribution
	std::uniform_int_distribution<unsigned int> distribution(0, int(genome_enc.size() - 1));
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
	std::iter_swap(genome_enc.begin() + pos1, genome_enc.begin() + pos2);
	
	// Store it back into the genome
	gen.genome_encoding_2b2_int = genome_enc;
}

void GAEncoding_Ass1::permutationSwap(Genome& gen, const uint32_t pos1, const uint32_t pos2)
{
	// Check pos1 and pos2 are not same
	if (pos1 == pos2)
	{
		std::cout << "Position 1 and Position 2 need to be different values." << std::endl;
	}

	// Get vector from genome
	std::vector<std::vector<int>> genome_enc = gen.genome_encoding_2b2_int;

	std::iter_swap(genome_enc.begin() + pos1, genome_enc.begin() + pos2);

	gen.genome_encoding_2b2_int = genome_enc;
}

Genome GAEncoding_Ass1::getGenomeFromPopulation(const unsigned int gen_num)
{
	return m_population[gen_num];
}