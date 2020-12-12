#pragma once
#include <SDL.h>
#include <stdio.h>
#include <string.h>
// A timer
typedef struct {
	Uint32 startTime, pauseTime;
	short paused;
	short started;
	char timeStr[30];
} Timer;

// Timer related functions
Timer* Timer_New() {
	Timer* newTimer = (Timer*)malloc(sizeof(Timer));
	if (newTimer == NULL) {
		printf("Could not allocate memory for Timer");
		return NULL;
	}
	newTimer->startTime = 0;
	newTimer->pauseTime = 0;
	newTimer->paused = newTimer->started = 0;
	return newTimer;
}

void Timer_Start(Timer* t) {
	t->started = 1;
	t->startTime = SDL_GetTicks();
}

Uint32 Timer_GetTime(Timer* t) {
	Uint32 timeResult = 0;
	if (t->started)
		timeResult = SDL_GetTicks() - t->startTime;
	if (t->paused)
		timeResult = t->pauseTime;
	return timeResult;
}

int Timer_Pause(Timer* t) {
	if (!t->started) { 
		return 0; 
	}
	if (!t->paused) {
		t->paused = 1;
		t->pauseTime = SDL_GetTicks() - t->startTime;
		return 1;
	}
	return 0;
}

int Timer_Resume(Timer* t) {
	if (!t->started) {
		return 0;
	}
	if (t->paused) {
		t->paused = 0;
		t->startTime = SDL_GetTicks() - t->pauseTime;
	}
	return 1;
}

char * Timer_GetStr(Timer* t) {
	if (!t->started) {
		strcpy_s(t->timeStr, 30, "Timer haven't been started!");
	}
	snprintf(t->timeStr, 30, "%lu", Timer_GetTime(t));
	return t->timeStr;
}

