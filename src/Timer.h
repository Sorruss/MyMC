#pragma once

#include <chrono>
#include <string>
#include <iostream>

class Timer
{
public:
	Timer(const std::string& name);
	~Timer();

public:
	void PrintResult() const;

private:
	std::string Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> StartPoint;
};
