#ifndef H_TETRIS_BLOCK
#define H_TETRIS_BLOCK
#include <SDL.h>
#include "LTexture.h"
typedef struct {
	int x, y;
	LTexture* texture;
} Block;
#endif // H_TETRIS_BLOCK