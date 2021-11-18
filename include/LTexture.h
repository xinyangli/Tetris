#ifndef H_TETRIS_LTEXTURE
#define H_TETRIS_LTEXTURE
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stdio.h>
#include "ConstNum.h"
// Define a wrapper struct for Texture
typedef struct {
	SDL_Texture* mTexture;
	int w, h;
} LTexture;

LTexture * newLTexture();
void emptyLTexture();
void renderLTexture();
int loadTexture();
int loadFromRenderedText(LTexture* dTexture, const char* textureText,
                         SDL_Renderer * renderer, TTF_Font * textFont,
                         SDL_Color textColor);

#endif // H_TETRIS_LTEXTURE