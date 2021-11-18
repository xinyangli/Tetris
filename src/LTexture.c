#include "LTexture.h"
LTexture* newLTexture() {
	LTexture* t = NULL;
	t = (LTexture*)malloc(sizeof(LTexture));
	if (t == NULL) {
		printf("Could not allocate memory for LTexture!");
		return NULL;
	}
	t->mTexture = NULL;
	t->w = t->h = 0;
	return t;
}

void emptyLTexture(LTexture* t) {
	//clear content in texture if it exist
	if (t->mTexture != NULL) {
		SDL_DestroyTexture(t->mTexture);
		t->w = 0; t->h = 0;
	}
}

void renderLTexture(SDL_Renderer* renderer,LTexture* texture, int x, int y) {
	SDL_Rect areaRect = { x - texture->w/2,y - texture->h/2,texture->w,texture->h };
	SDL_RenderCopy(renderer, texture->mTexture, NULL, &areaRect);
}

int loadTexture(LTexture* dTexture, const char* path, SDL_Renderer* renderer) {
	emptyLTexture(dTexture);
	SDL_Surface* loadedSurface = NULL;
	loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL) {
		printf("Could not load image:%s! IMG_Error:%s", path, IMG_GetError());
		return 0;
	}
	dTexture->mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	dTexture->w = loadedSurface->w; dTexture->h = loadedSurface->h;
	SDL_FreeSurface(loadedSurface);
	if (dTexture->mTexture == NULL) {
		printf("Could not create texture from surface! SDL_Error: %s", SDL_GetError());
		free(dTexture);
		return 0;
	}
	return 1;
}

int loadFromRenderedText(LTexture* dTexture, const char* textureText, SDL_Renderer * renderer, TTF_Font * textFont, SDL_Color textColor) {
	emptyLTexture(dTexture);
	SDL_Surface* textSurface = TTF_RenderText_Blended(textFont, textureText, textColor);
	if (textSurface == NULL) {
		printf("Could not render text: %s! TTF_Error: %s", textureText, TTF_GetError());
		return 0;
	}
	dTexture->mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	dTexture->w = textSurface->w; dTexture->h = textSurface->h;
	SDL_FreeSurface(textSurface);
	if (dTexture->mTexture == NULL) {
		printf("Could not convert text \"%s\"to texture! SDL_Error: %s", textureText, SDL_GetError());
		return 0;
	}
	return 1;
}
