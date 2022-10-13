// Christopher Neves: 27521979 / Massimo Pietracupa: 27313683
#ifndef GENOME_H_
#define GENOME_H_

#include <vector>
#include <string>

class Genome
{
private:
	int fitness = 0;
	int row_mismatches = 0;
	int col_mismatches = 0;
public:
	std::vector<std::vector<int>> genome_encoding_2b2_int;

	Genome() {};
	inline Genome(std::vector<std::vector<int>> vec) { this->genome_encoding_2b2_int = vec; }

	// Getters
	size_t getSize() const;
	int getFitness() const { return fitness; }
	int getRowMismatches() const { return row_mismatches; }
	int getColMismatches() const { return col_mismatches; }
	std::string getGenomeString();
	const std::vector<std::vector<int>>&  getEncoding() const { return genome_encoding_2b2_int; }

	// Setters
	void setFitness(std::vector<int> f);

	bool operator< (const Genome& other) const { return fitness < other.fitness; }
	bool operator== (const Genome& rhs) { return (fitness == rhs.fitness) && (genome_encoding_2b2_int == rhs.genome_encoding_2b2_int); }
};

#endif // !GENOME_H_

