#include "NetworkStatistics.h"

bool NetworkStatistics::checkStagnation(int currentFitness, int generation)
{

	if (currentFitness == previousFitness)
	{
		fitnessRepeats++;
	}
	else {
		fitnessRepeats = 0;
	}

	previousFitness = currentFitness;

	if (fitnessRepeats > stagnation_window)
	{
		stagnation_detected = true;
		fitnessRepeats = 0;
	}
	else {
		stagnation_detected = false;
	}

	return stagnation_detected;

}

void NetworkStatistics::tuneParameter(float& parameter, const float tune, const float clamp_min, const float clamp_max)
{
	if (parameter == 0.0f && tune > 1.0f)
	{
		parameter = 0.1f;
	}
	if (parameter > 0.0f && tune > 1.0f)
	{
		parameter += 0.05f;
	}

	parameter *= tune;
	parameter = std::clamp(parameter, clamp_min, clamp_max);
}

NetworkStatistics::NetworkStatistics()
{
	stagnation_detected = false;
	m_stagnation_modified_countdown = -1;
}

void NetworkStatistics::setStagnationWindow(int stagnation_window)
{
	this->stagnation_window = stagnation_window;
}