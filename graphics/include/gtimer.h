#ifndef _GAME_TIMER_H_
#define _GAME_TIMER_H_

typedef struct _GameTimer
{
	double _secondsPerCount;
	double _deltaTime;

	__int64 _baseTime;
	__int64 _prevTime;
	__int64 _stopTime;
	__int64 _pausedTime;
	__int64 _currTime;

	bool _stopped;
} GameTimer;

void GameTimer_Init(GameTimer* gt);

void GameTimer_Reset(GameTimer* gt);
void GameTimer_Start(GameTimer* gt);
void GameTimer_Stop(GameTimer* gt);
void GameTimer_Tick(GameTimer* gt);

float GameTimer_DeltaTime(GameTimer* gt);
float GameTimer_TotalTime(GameTimer* gt);

#endif /* _GAME_TIMER_H_ */
