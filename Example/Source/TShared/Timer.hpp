#pragma once

#include <chrono>

namespace CU
{
	class StopWatch
	{
	public:
		StopWatch();
		~StopWatch() = default;
		StopWatch(const StopWatch& aTimer) = default;
		StopWatch& operator=(const StopWatch& aTimer) = default;

		void Stop();
		void Reset();
		float GetDeltaSeconds() const;
		float GetDeltaMilliSeconds() const;
	private:
		std::chrono::high_resolution_clock::time_point myStartTime;
		std::chrono::duration<float> myDeltaTime;
	};

	class FrameTimer
	{
	public:
		FrameTimer();
		~FrameTimer() = default;
		FrameTimer(const FrameTimer& aTimer) = delete;
		FrameTimer& operator=(const FrameTimer& aTimer) = delete;

		void Update();

		float GetDeltaTime() const;
		double GetTotalTime() const;
	private:
		std::chrono::high_resolution_clock::time_point myCreationTime;
		std::chrono::high_resolution_clock::time_point myCurrentFrame;
		std::chrono::high_resolution_clock::time_point myLastFrame;
		std::chrono::duration<float> myDeltaTime;
		float myTimeScale;
	};

	class GenericTimer
	{
	public:
		GenericTimer();
		GenericTimer(float aDuration);
		~GenericTimer() = default;
		GenericTimer(const GenericTimer& aCopy) = default;
		GenericTimer& operator=(const GenericTimer& aOther) = default;

		void Update(float aDeltaTime);
		void Start(float aDuration);
		bool IsDone() const;
		float GetProgress() const;
	private:
		float myTime;
		float myDuration;
	};
}