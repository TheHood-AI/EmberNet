#pragma once
#include <chrono>

namespace EmberNet
{
	namespace CU
	{

		class CTimer
		{
		public:
			CTimer();
			~CTimer() = default;
			CTimer(const CTimer& aTimer) = delete;
			CTimer& operator=(const CTimer& aTimer) = delete;

			void Update();

			float GetDeltaTime() const;
			double GetTotalTime() const;
		private:
			std::chrono::high_resolution_clock::time_point myCreationTime;
			std::chrono::high_resolution_clock::time_point myCurrentFrame;
			std::chrono::high_resolution_clock::time_point myLastFrame;
			std::chrono::duration<float> myDeltaTime;
		};

	class CStopWatch
	{
	public:
		CStopWatch();
		~CStopWatch() = default;
		CStopWatch(const CStopWatch& aTimer) = default;
		CStopWatch& operator=(const CStopWatch& aTimer) = default;

		void Stop();
		void Reset();
		float GetDeltaSeconds() const;
		float GetDeltaMilliSeconds() const;
	private:
		std::chrono::high_resolution_clock::time_point myStartTime;
		std::chrono::duration<float> myDeltaTime;
	};
	}
}

