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
}


std::vector<std::vector<int>> vectTo2D(std::vector<int> vect_input, uint32_t row, uint32_t col)
{
	std::vector<std::vector<int>> output;
	std::vector<int> row_vect;

	int counter = 0;
	for (unsigned int i = 0; i < row; i++)
	{
		for (unsigned int j = 0; j < col; j++)
		{
			if (counter > vect_input.size())
			{
				row_vect.push_back(0);
			}
			else
			{
				row_vect.push_back(vect_input[counter]);
				counter++;
			}
		}

		output.push_back(row_vect);
		row_vect.clear();
	}

	return output;
}

std::vector<std::vector<int>> vectSlide(std::vector<std::vector<int>>vect_input, std::vector<std::vector<int>> indices, int slide, bool vertical)
{
	std::vector<std::vector<int>> sliding_box = vect_input;
	std::vector<int> row_vect;

	int bounding_box_width = indices[1][1] - indices[0][1];
	int bounding_box_height = indices[1][0] - indices[0][0];
	// Indices should be
	// R1C1
	// R2C2

	// copy the second bounding box into its position in sliding_box
	int starting_column = indices[0][1] + slide;
	int starting_row = indices[0][0] + slide;
	int ending_column = starting_column + bounding_box_width;
	int ending_row = starting_row + bounding_box_height;

	if (bounding_box_height - slide > 0)
	{
		starting_row += bounding_box_height - slide;
	}

	if (bounding_box_width - slide > 0)
	{
		starting_column += bounding_box_width - slide;
	}

	if (!vertical)
	{
		// this is for horizontal slide
		// Copy the non bounding box portion
		for (int i = indices[0][0]; i < indices[1][0] + bounding_box_height; i++)
		{
			for (int j = starting_column; j <= ending_column; j++)
			{
				sliding_box[i][indices[0][1] + (j - starting_column)] = vect_input[i][j];
				
			}
		}
		// Copy the bounding box
		for (int i = indices[0][0]; i <= indices[1][0]; i++)
		{
			for (int j = indices[0][1] + slide; j <= indices[1][1] + slide; j++)
			{
				sliding_box[i][j] = vect_input[i][j - slide];
			}
		}
	}
	else
	{
		// this is for horizontal slide
		// Copy the non bounding box portion
		for (int i = starting_row; i <= ending_row; i++)
		{
			for (int j = indices[0][1]; j <= indices[1][1]; j++)
			{
				sliding_box[indices[0][0] + (i - starting_row)][j] = vect_input[i][j];
			}
		}
		// Copy the bounding box
		for (int i = indices[0][0] + slide; i <= indices[1][0] + slide; i++)
		{
			for (int j = indices[0][1]; j <= indices[1][1]; j++)
			{
				sliding_box[i][j] = vect_input[i - slide][j];
			}
		}
	}

	return sliding_box;
}


std::vector<unsigned int> getBoundingBox(int col_dim, int row_dim, std::mt19937& engine, int index1, int index2, int max_area, int max_cols, int max_rows)
{
	
	// If indices are not given
	std::uniform_int_distribution distr(0, col_dim * row_dim - 1);
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
	if (num_cols < 0)
	{
		// Case where column num of index 1 is greater than index 2
		num_cols = abs(num_cols);
		index1 = index1 - num_cols;
		index2 = index2 + num_cols;
	}

	int num_rows = index2 / col_dim - index1 / col_dim;

	std::vector<unsigned int> indices{};


	int rows_left = max_rows;
	int cols_left = max_cols;

	for (int i = 0; i <= num_rows; i++)
	{
		for (int j = 0; j <= num_cols; j++)
		{
			
			indices.push_back(col_dim * i + j + index1);

			if ((max_area != -1) && indices.size() == max_area)
			{
				return indices;
			}

			if ((max_cols != -1) && (max_cols == (j + 1)))
			{
				break;
			}
		}

		if ((max_rows != -1) && (max_rows == (i + 1)))
		{
			break;
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


void savePhenotypeToFile(std::string p, std::string filename)
{
	std::ofstream output(filename);
	output << p;
	output.close();
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

