#ifndef FOUNDATION_TIMER_HPP
#define FOUNDATION_TIMER_HPP

#include "Foundation/Object.hpp"
#include <chrono>
#include <thread>
#include <functional>

// IMPLEMENT SOMETHING PLATFORM INDEPENDENT
// Non-multi thread safe!
// More info: https://github.com/eglimi/cpptime/blob/master/cpptime.h

namespace GraphicsEngine
{
	class Timer : public Object
	{
		GE_RTTI(GraphicsEngine::Timer)

	public:
		typedef std::function<void()> OnTimeOutFunc;

		Timer();
		explicit Timer(int64_t timeOutInterval, const OnTimeOutFunc& func = {}, bool_t isRepeat = false);
		virtual ~Timer();

		void Start();
		void Restart();
		void Stop();

		int64_t ElapsedTimeInNanoseconds();
		int64_t ElapsedTimeInMicroseconds();
		int64_t ElapsedTimeInMiliseconds();
		int64_t ElapsedTimeInSeconds();

		bool_t IsRunning() const;
		bool_t IsRepeat() const;
		int64_t TimeoutInterval() const;

	private:
		NO_COPY_NO_MOVE_CLASS(Timer)

		std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime, mEndTime;
		bool_t mIsRunning;
		bool_t mIsRepeat;
		int64_t mTimeOutInterval;

		std::thread mWorker;
		OnTimeOutFunc mOnTimeOut;

		void Run();
	};
}
#endif /* FOUNDATION_TIMER_HPP */