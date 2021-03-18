#pragma once

#include <Windows.h>
#include <profileapi.h>
#include <string>
#include <iostream>

class StopWatch
{
public:

	static StopWatch& getInstance()
	{
		static StopWatch* instance = nullptr;
		if (!instance)
			instance = new StopWatch;

		return *instance;
	}

	void start()
	{
		QueryPerformanceCounter(&mStart);
	}

	void stop()
	{
		QueryPerformanceCounter(&mEnd);
	}

	double getElapsed()
	{
		
		double dur = (double)(mEnd.QuadPart - mStart.QuadPart) * mTicksPerSecond;
		return dur;
	}

	void printElapsed(double elapsed = -1.0)
	{
		std::string str;
		double dur = 0.0;
		if (elapsed == -1.0)
			dur = (double)(mEnd.QuadPart - mStart.QuadPart) * mTicksPerSecond;
		else
			dur = elapsed;

		if (dur > 1.0)
			str = "s";
		else if (dur < 1.0 && dur > 0.001)
		{
			str = "ms";
			dur *= 1e3;
		}
		else if (dur < 0.001 && dur > 0.000001)
		{
			str = "us";
			dur *= 1e6;
		}
		else if (dur < 0.000001)
		{
			str = "ns";
			dur *= 1e9;
		}

		std::cout << dur << str << std::endl;

	}

private:

	StopWatch()
		:
		mStart(),
		mEnd()
	{
		LARGE_INTEGER ticks;
		QueryPerformanceFrequency(&ticks);
		mTicksPerSecond = 1.0 / (double)ticks.QuadPart;
	}

	~StopWatch(){}


	LARGE_INTEGER mStart, mEnd;
	double mTicksPerSecond;

	
};