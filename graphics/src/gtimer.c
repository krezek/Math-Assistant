/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"


#include <gtimer.h>

void GameTimer_Init(GameTimer* gt)
{
	gt->_secondsPerCount = 0.0;
	gt->_deltaTime = -1.0;
	gt->_baseTime = 0;
	gt->_prevTime = 0;
	gt->_stopTime = 0;
	gt->_pausedTime = 0;
	gt->_currTime = 0;

	__int64 countPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*) & countPerSecond);
	gt->_secondsPerCount = 1.0 / countPerSecond;
}

void GameTimer_Reset(GameTimer* gt)
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	gt->_baseTime = currTime;
	gt->_prevTime = currTime;
	gt->_stopTime = 0;
	gt->_stopped = false;
}

void GameTimer_Start(GameTimer* gt)
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (gt->_stopped)
	{
		gt->_pausedTime += startTime - gt->_stopTime;

		gt->_prevTime = startTime;
		gt->_stopTime = 0;
		gt->_stopped = false;
	}
}

void GameTimer_Stop(GameTimer* gt)
{
	if (!gt->_stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		gt->_stopTime = currTime;
		gt->_stopped = true;
	}
}

void GameTimer_Tick(GameTimer* gt)
{
	if (gt->_stopped)
	{
		gt->_deltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	gt->_currTime = currTime;

	gt->_deltaTime = (gt->_currTime - gt->_prevTime) * gt->_secondsPerCount;

	gt->_prevTime = gt->_currTime;

	if (gt->_deltaTime < 0.0)
	{
		gt->_deltaTime = 0.0;
	}
}

float GameTimer_DeltaTime(GameTimer* gt)
{
	return (float)gt->_deltaTime;
}

float GameTimer_TotalTime(GameTimer* gt)
{
	if (gt->_stopped)
		return (float)(((gt->_stopTime - gt->_pausedTime) - gt->_baseTime) * gt->_secondsPerCount);
	else
		return (float)(((gt->_currTime - gt->_pausedTime) - gt->_baseTime) * gt->_secondsPerCount);
}
