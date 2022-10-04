#include "timer.h"

void Stopwatch::Start()
{
	start = std::chrono::steady_clock::now();
}

double Stopwatch::Stop()
{
	stop = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = stop - start;

	return elapsed_seconds.count();
}

Stopwatch::Stopwatch()
{

}

Stopwatch::~Stopwatch()
{

}