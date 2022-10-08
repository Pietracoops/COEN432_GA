#ifndef GENOME_H_
#define GENOME_H_

#include <vector>
#include <string>

class Genome
{
private:
	int fitness = 0;
public:
	Genome() {};
	inline Genome(std::vector<std::vector<int>> vec) { this->genome_encoding_2b2_int = vec; }
	std::vector<std::vector<int>> genome_encoding_2b2_int;
	size_t getSize() const;
	void setFitness(int f) { fitness = f; }
	int getFitness() const { return fitness; }
	std::string getGenomeString();
	std::vector<std::vector<int>> getEncoding() const { return genome_encoding_2b2_int; }

	bool operator< (const Genome& other) const { return fitness < other.fitness; }
	bool operator== (const Genome& rhs) { return (fitness == rhs.fitness) && (genome_encoding_2b2_int == rhs.genome_encoding_2b2_int); }
};

#endif // !GENOME_H_

