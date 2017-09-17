#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer();

	void Reset();
	void Start();
	void Stop();
	void Tick();

	float GameTime() const;
	float DeltaTime() const;

private:

	double m_SecondsPerCount;
	double m_DeltaTime;

	bool m_Stopped;

	__int64 m_BaseCount;
	__int64 m_PreCount;
	__int64 m_CurCount;
	__int64 m_PausedCount;
	__int64 m_DeltaBetweenStopAndPre;
};