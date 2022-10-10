#include "helper_functions.h"

std::vector<float> generateRandVecFloat(int n_elem, std::mt19937& engine)
{
	// Create the distribution object
	std::uniform_real_distribution<float> dist(0, 1);

	// Bind the engine parameter to the dist function call
	auto gen = std::bind(dist, engine);

	// Create a vector to hold all the random floats
	std::vector<float> vec(n_elem);

	// Generate the values
	std::generate(vec.begin(), vec.end(), gen);

	return vec;

}

std::vector<std::string> readFileIntoMemory(std::string file_path)
{
    std::ifstream infile;
    std::filesystem::path p = file_path;          // Get file path
    infile.open(std::filesystem::absolute(p));    // Open File using absolute path
    std::string line;                             // Init a string to store the line
    std::vector<std::string> output;              // Init output string vector

    while (std::getline(infile, line))
    {
        output.push_back(line); // Store the line
    }

    infile.close(); // Close the file
    
    return output;
}

std::vector<std::vector<int>> shuffleVector(std::vector<std::vector<int>> vect)
{
	std::vector<std::vector<int>> vect_tmp = vect;
	auto rd = std::random_device{};
	auto rng = std::default_random_engine{ rd() };
	std::shuffle(std::begin(vect_tmp), std::end(vect_tmp), rng);

	return vect_tmp;
}

std::vector<int> shuffleVector(std::vector<int> vect)
{
	std::vector<int> vect_tmp = vect;
	auto rd = std::random_device{};
	auto rng = std::default_random_engine{ rd() };
	std::shuffle(std::begin(vect_tmp), std::end(vect_tmp), rng);

	return vect_tmp;
}

std::vector<Genome> shuffleVector(std::vector<Genome> vect)
{
	std::vector<Genome> vect_tmp = vect;
	auto rd = std::random_device{};
	auto rng = std::default_random_engine{ rd() };
	std::shuffle(std::begin(vect_tmp), std::end(vect_tmp), rng);

	return vect_tmp;
}


std::vector<std::string> splitString(std::string str, std::string delimiter)
{
	std::vector<std::string> output;
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		output.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	output.push_back(str);
	return output;

std::vector<int> getBoundingBox(int col_dim, int row_dim, std::mt19937 engine, int index1, int index2, int max_area)
{
	
	// If indices are not given
	std::uniform_int_distribution distr(0, col_dim * row_dim);
	if (index1 == -1)
	{
		index1 = distr(engine);
	}
	if (index2 == -1)
	{
		index2 = distr(engine);
	}

	// Sort indices in increasing value
	std::vector<int> temp = { index1, index2 };
	std::sort(temp.begin(), temp.end());
	index1 = temp[0];
	index2 = temp[1];

	// Get number of rows and columns separating the two indices
	int num_cols = (index2 % col_dim - index1 % col_dim);
	int num_rows = index2 / col_dim - index1 / col_dim;

	std::vector<int> indices{};


	for (int i = 0; i <= num_rows; i++)
	{
		for (int j = 0; j <= num_cols; j++)
		{
			indices.push_back(col_dim * i + j + index1);

			if ((max_area != -1) && indices.size() == max_area)
			{
				return indices;
			}
		}
	}

	return indices;

}

void printVector(std::vector<std::vector<int>> vect)
{
	for (unsigned int i = 0; i < vect.size(); i++)
	{
		std::cout << vect[i][0] << ":" << vect[i][1] << "-";
	}
	std::cout << std::endl;
}

int CountRowMismatches(std::vector<std::string> first, std::vector<std::string> second, int row)
{
	int numberOfMismatches = 0;

	for (int i = 0; i < first.size(); i++)
	{
		auto one = first[i].at(2);
		auto two = second[i].at(0);
		if (first[i].at(2) != second[i].at(0))
		{
			//std::cout << "PROF: mismatch between row[" << row << "] col[" << i << "]" << std::endl;
			numberOfMismatches++;
		}
			
	}


	return numberOfMismatches;
}

int CountColumnMismatches(std::vector<std::string> first, std::vector<std::string> second)
{
	int numberOfMismatches = 0;

	for (int i = 0; i < first.size(); i++)
		if (first[i].at(1) != second[i].at(3))
			numberOfMismatches++;

	return numberOfMismatches;
}


int ProfTest(std::string file_path)
{
	std::ifstream inputFile;

	std::filesystem::path p = file_path;          // Get file path
	inputFile.open(std::filesystem::absolute(p));

	std::string currentText;
	std::getline(inputFile, currentText);

	std::vector<std::vector<std::string>> puzzlePieces;
	std::string currentNumber;

	const int RowSize = 8;
	const int ColumnSize = 8;
	const char delimiter = ' ';

	for (int i = 0; i < RowSize; i++)
	{
		std::vector<std::string> row;

		getline(inputFile, currentText);

		std::stringstream currentLine(currentText);

		while (std::getline(currentLine, currentNumber, delimiter))
			row.push_back(currentNumber);

		puzzlePieces.push_back(row);
	}

	int numberOfMismatches = 0;

	for (int i = 0; i < RowSize - 1; i++)
		numberOfMismatches += CountRowMismatches(puzzlePieces[i], puzzlePieces[i + 1], i);

	for (int i = 0; i < ColumnSize - 1; i++)
	{
		std::vector<std::string> firstColumn;
		std::vector<std::string> secondColumn;

		for (int j = 0; j < RowSize; j++)
		{
			firstColumn.push_back(puzzlePieces[j][i]);
			secondColumn.push_back(puzzlePieces[j][i + 1]);
		}

		numberOfMismatches += CountColumnMismatches(firstColumn, secondColumn);
	}

	std::cout << "Number of mismatches : " << numberOfMismatches << std::endl;

	inputFile.close();

	return 0;
}

