#include "Timer.h"

Timer::Timer(const std::string& name) : Name(name)
{
	StartPoint = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
	PrintResult();
}

void Timer::PrintResult() const
{
	auto nowPoint	= std::chrono::high_resolution_clock::now();
	auto now		= std::chrono::time_point_cast<std::chrono::microseconds>(nowPoint).time_since_epoch().count();
	auto start		= std::chrono::time_point_cast<std::chrono::microseconds>(StartPoint).time_since_epoch().count();
	
	double microseconds = now - start;
	double milliseconds = microseconds * 0.001;
	
	std::cout << "[TIMER:" + Name + "]: " << milliseconds << " ms\n";
}
