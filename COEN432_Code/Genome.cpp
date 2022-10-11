#include "Genome.h"

std::string Genome::getGenomeString()
{
	std::string output;
	for (auto gene : genome_encoding_2b2_int)
	{
		output += "gene: " + std::to_string(gene[0]) + ";rot: " + std::to_string(gene[1]) + ";";
	}

	return output;
}

void Genome::setFitness(std::vector<int> f)
{ 
	fitness = f[0];
	row_mismatches = f[1];
	col_mismatches = f[2]; 

}