#pragma once

#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>

class Stopwatch
{
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point stop;
public:
	Stopwatch();
	~Stopwatch();

	void Start();
	double Stop();

};

#endif