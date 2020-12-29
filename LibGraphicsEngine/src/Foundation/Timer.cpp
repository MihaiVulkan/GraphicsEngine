#include "Timer.hpp"
#include <cmath>
#include <iostream>

using namespace GraphicsEngine;

Timer::Timer()
	: mIsRunning(false)
	, mIsRepeat(false)
	, mTimeOutInterval(0)
	, mStartTime()
	, mEndTime()
	, mWorker()
	, mOnTimeOut(nullptr)
{}

Timer::Timer(int64_t timeOutInterval, const OnTimeOutFunc& func, bool_t isRepeat)
	: mIsRunning(false)
	, mIsRepeat(isRepeat)
	, mTimeOutInterval(timeOutInterval)
	, mStartTime()
	, mEndTime()
	, mWorker()
	, mOnTimeOut(func)
{
	mWorker = std::thread([this] { Run(); });
}

Timer::~Timer()
{
	// we let it run until it finishes
	// otherwise if mIsRunning becomes false
	// then it may exit Run() earlier then the
	// expected timeout interval !

	if (mWorker.joinable())
		mWorker.join();
}

void Timer::Start()
{
	if (mIsRunning)
		return;

	mIsRunning = true;
	mStartTime = std::chrono::high_resolution_clock::now();

//	std::cout << "Timer Start!" << std::endl;
}

void Timer::Restart()
{
//	std::cout << "Timer Restart!" << std::endl;

	if (mIsRunning)
	{
		Stop();
	}
	else
	{
		Start();
	}

}

void Timer::Stop()
{
	if (false == mIsRunning)
		return;

	mEndTime = std::chrono::high_resolution_clock::now();
	mIsRunning = false;

//	std::cout << "Timer Stop!" << std::endl;
}

void Timer::Run()
{
	while (mIsRunning)
	{
		// Timeout

		float64_t timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - mStartTime).count();
	
		if (timeDiff >= static_cast<float64_t>(mTimeOutInterval))
		{
			if (mOnTimeOut)
				mOnTimeOut();

			if (mIsRepeat)
			{
				mStartTime = std::chrono::high_resolution_clock::now();
			}
			else
			{
				Stop();
			}
		}
	}
}

int64_t Timer::ElapsedTimeInNanoseconds()
{
	int64_t elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(mEndTime - mStartTime).count();

	return elapsedTime;
}

int64_t Timer::ElapsedTimeInMicroseconds()
{
	int64_t elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(mEndTime - mStartTime).count();

	return elapsedTime;
}

int64_t Timer::ElapsedTimeInMiliseconds()
{
	int64_t elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(mEndTime - mStartTime).count();

	return elapsedTime;
}

int64_t Timer::ElapsedTimeInSeconds()
{
	int64_t elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(mEndTime - mStartTime).count();

	return elapsedTime;
}

bool_t Timer::IsRunning() const
{
	return mIsRunning;
}

bool_t Timer::IsRepeat() const
{
	return mIsRepeat;
}

int64_t Timer::TimeoutInterval() const
{
	return mTimeOutInterval;
}