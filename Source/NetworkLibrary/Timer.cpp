#include "pch.h"
#include "Timer.h"

namespace EmberNet::CU
{
	CTimer::CTimer() :
		myDeltaTime(std::chrono::duration<float>(0.0f))
	{
		myCreationTime = std::chrono::high_resolution_clock::now();
		myLastFrame = myCreationTime;
	}

	void CTimer::Update()
	{
		myCurrentFrame = std::chrono::high_resolution_clock::now();
		myDeltaTime = myCurrentFrame - myLastFrame;
		myLastFrame = myCurrentFrame;
	}

	float CTimer::GetDeltaTime() const
	{
		return myDeltaTime.count();
	}

	double CTimer::GetTotalTime() const
	{
		return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - myCreationTime).count();
	}

	CStopWatch::CStopWatch() :
		myDeltaTime(std::chrono::duration<float>(0.0f))
	{
		Reset();

	}

	void CStopWatch::Stop()
	{
		myDeltaTime = std::chrono::high_resolution_clock::now() - myStartTime;
	}

	void CStopWatch::Reset()
	{
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	float CStopWatch::GetDeltaSeconds() const
	{
		return myDeltaTime.count();
	}

	float CStopWatch::GetDeltaMilliSeconds() const
	{
		return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(myDeltaTime).count());
	}
}
