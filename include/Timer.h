#ifndef H_TETRIS_TIMER
#define H_TETRIS_TIMER
#include <SDL.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	Uint32 startTime, pauseTime;
	short paused;
	short started;
	char timeStr[30];
} Timer;

Timer * Timer_New();
void Timer_Start(Timer * t);
Uint32 Timer_GetTime(Timer* t);
int Timer_Pause(Timer* t);
int Timer_Resume(Timer* t);
char * Timer_GetStr(Timer* t);

#endif // H_TETRIS_TIMER