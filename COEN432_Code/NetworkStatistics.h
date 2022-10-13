#pragma once


#ifndef NETWORK_STATISTICS_H_
#define NETWORK_STATISTICS_H_

#include "helper_functions.h"

class NetworkStatistics
{

private:
	int stagnation_window = 10;
	int fitnessRepeats = 0;
	int previousFitness = 0;
public:
	NetworkStatistics();
	std::vector<int> m_max_fitness;
	std::vector<int> m_med_fitness;
	std::vector<int> m_min_fitness;

	int m_stagnation_countdown_val = 10;
	int m_stagnation_modified_countdown;
	bool stagnation_detected;
	/*bool checkStagnation(int generation_range, int generation, int breath);*/
	bool checkStagnation(int currentFitness, int generation);
	void tuneParameter(float& parameter, const float tune, const float clamp_min, const float clamp_max);
	void setStagnationWindow(int stagnation_window);
};

#endif // NETWORK_STATISTICS_H_