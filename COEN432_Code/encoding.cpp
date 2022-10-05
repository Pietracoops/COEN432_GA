
#include "encoding.h"


void GAEncoding::initializaPopulation(const unsigned int number_of_genomes)
{

}

// Core Functions
void GAEncoding::parentSelection()
{

}
void GAEncoding::recombination()
{

}
void GAEncoding::survivorSelection()
{

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
		std::vector<std::vector<int>> randomized_genome = shuffleVector(m_original_genome);
		m_population.push_back(randomized_genome);
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