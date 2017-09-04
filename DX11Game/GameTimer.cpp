#include "GameTimer.h"
#include <windows.h>

GameTimer::GameTimer()
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	m_SecondsPerCount = 1.0 / (double)countsPerSecond;
}


GameTimer::~GameTimer()
{
}

void GameTimer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurCount);
	m_BaseCount = m_CurCount;
	m_PreCount = m_CurCount;
	m_PausedCount = 0;
	m_Stopped = false;
}

void GameTimer::Start()
{
	if (!m_Stopped) {
		return;
	}
	__int64 stoppedTime = m_CurCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurCount);
	m_PausedCount += m_CurCount - stoppedTime;

	m_PreCount = m_CurCount - m_DeltaBetweenStopAndPre;

	m_Stopped = false;

}

void GameTimer::Stop()
{
	if (m_Stopped) {
		return;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurCount);

	m_DeltaBetweenStopAndPre = m_CurCount - m_PreCount;
	m_Stopped = true;

}

void GameTimer::Tick()
{
	if (m_Stopped) {
		m_DeltaTime = 0;
		return;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurCount);
	m_DeltaTime = (m_CurCount - m_PreCount)*m_SecondsPerCount;

	m_PreCount = m_CurCount;

	if (m_DeltaTime < 0.0) {
		m_DeltaTime = 0.0;
	}
}

float GameTimer::GameTime() const
{
	return static_cast<float>((m_CurCount - m_BaseCount - m_PausedCount)*m_SecondsPerCount);
}

float GameTimer::DeltaTime() const
{
	return static_cast<float>(m_DeltaTime);
}