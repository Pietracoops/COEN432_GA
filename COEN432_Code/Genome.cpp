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