#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>
#include "ConstNum.h"
#include "LTexture.h"
#include "Timer.h"
#include "Block.h"
#define MAX(x, y)  ((x) > (y) ? (x) : (y))
void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

// Global variables for window
SDL_Window* gWindow = NULL; /*window to display things on*/
SDL_Renderer* gRenderer = NULL; /*The renderer will be used*/
Timer* gTimer = NULL; /*Timer is used to manage time in game*/
LTexture* gBlocks[PIECE_TYPE_CNT] = { NULL };
const char* blocksResourceName[PIECE_TYPE_CNT] = { "purple", "orange", "blue", "cyan", "red", "green", "yellow" };
LTexture* gDropBlock = NULL;
LTexture* gScore = NULL; int gScoreNum = 0;
TTF_Font* gFont = NULL;
TTF_Font* gInputFont = NULL;
LTexture* gTitleTexture = NULL;
LTexture* gBackgroundTexture = NULL;
LTexture* gRankTexture = NULL;

/* Devide the screen into several rectangles */
SDL_Rect gGameRect = { SCREEN_CENTER_X-AREA_GAME_WIDTH/2, 0, AREA_GAME_WIDTH,SCREEN_HEIGHT };
SDL_Rect gScoreRect = { AREA_SCORE_CENTER_X - AREA_SCORE_WIDTH / 2, AREA_SCORE_CENTER_Y - AREA_SCORE_HEIGHT / 2,
						AREA_SCORE_WIDTH, AREA_SCORE_HEIGHT };
SDL_Rect gNextBlockRect = { AREA_NEXTBLOCK_CENTER_X - AREA_NEXTBLOCK_WIDTH / 2, AREA_NEXTBLOCK_CENTER_Y - AREA_NEXTBLOCK_HEIGHT / 2,
						AREA_NEXTBLOCK_WIDTH, AREA_NEXTBLOCK_HEIGHT };
SDL_Rect gScreenRect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

char gRankPlayer[5][MAX_USERNAME+5];
char gCurrentPlayer[MAX_USERNAME+5];
int gRankScore[5];
FILE* gRankFile = NULL;
int gGameState = 0; /*0 means game stopped, 1 means game paused, 2 means game running*/
int dead = 0;
/*------- Variables used --------*/
Uint32 lastMoveTime = 0;
const Uint8* keyboardState = NULL;
Uint32 tickInterval = 300;
/*-------------------------------*/
int level = 1, levelDiff = 100, fastFowardInterval = 60, normalInterval = 300;

Block blockGrid[BLOCKS_IN_HEIGHT + BLOCKS_GRID_FIRSTLINE][BLOCKS_IN_WIDTH] = { 0 };


/* Define pieces in this section*/
/* Select a central block in each type of piece,
   define other blocks using the delta value */
// int delta_x[PIECE_TYPE_CNT][4] = { { 0, 1, 0,-1},{ 0, 1,-1,-1},{ 0, 1, 1,-1},{ 0, 0, 0, 0},{ 0, 1, 0,-1},{ 0, 1, 0,-1} };
// int delta_y[PIECE_TYPE_CNT][4] = { { 0, 0,-1, 0},{ 0, 0,-1, 0},{ 0, 0,-1, 0},{ 0, 1,-1,-2},{ 0, 0,-1,-1},{ 0,-1,-1, 0} };

int delta_x[PIECE_TYPE_CNT][4][4] = {
	{{ 0, 1, 0,-1},{ 0, 0, 1, 0},{ 0,-1, 0, 1},{ 0, 0,-1, 0}},
	{{ 0, 1,-1,-1},{ 0, 0, 1, 0},{ 0,-1, 1, 1},{ 0, 0,-1, 0}},
	{{ 0, 1, 1,-1},{ 0, 0, 1, 0},{ 0,-1,-1, 1},{ 0, 0,-1, 0}},
	{{ 0, 0, 0, 0},{ 0,-1, 1, 2},{ 0, 0, 0, 0},{ 0, 1,-1,-2}},
	{{ 0, 1, 0,-1},{ 0, 0, 1, 1},{ 0,-1, 0, 1},{ 0, 0,-1,-1}},
	{{ 0, 1, 0,-1},{ 0, 1, 1, 0},{ 0,-1, 0, 1},{ 0,-1,-1, 0}},
	{{ 0, 0,-1,-1},{ 0, 0,-1,-1},{ 0, 0,-1,-1},{ 0, 0,-1,-1}}
};
int delta_y[PIECE_TYPE_CNT][4][4] = {
	{{ 0, 0,-1, 0},{ 0, 1, 0,-1},{ 0, 0, 1, 0},{ 0,-1, 0, 1}},
	{{ 0, 0,-1, 0},{ 0, 1,-1,-1},{ 0, 0, 1, 0},{ 0,-1, 1, 1}},
	{{ 0, 0,-1, 0},{ 0, 1, 1,-1},{ 0, 0, 1, 0},{ 0,-1,-1, 1}},
	{{ 0, 1,-1,-2},{ 0, 0, 0, 0},{ 0,-1, 1, 2},{ 0, 0, 0, 0}},
	{{ 0, 0,-1,-1},{ 0, 1, 0,-1},{ 0, 0, 1, 1},{ 0,-1, 0, 1}},
	{{ 0,-1,-1, 0},{ 0, 1, 0,-1},{ 0, 1, 1, 0},{ 0,-1, 0, 1}},
	{{ 0,-1,-1, 0},{ 0,-1,-1, 0},{ 0,-1,-1, 0},{ 0,-1,-1, 0}}
};

/*4 indicates that there are 4 blocks in each piece */

typedef struct {
	int type, angle;
	/*
	------------------ TYPE DESCRIPTION ------------------ 
	Type 0:  Type 1:  Type 2:  Type 3:  Type 4:  Type 5:  Type 6:
	   *      *            *      *      * *        * *    * *
	 *[*]*    *[*]*    *[*]*     [*]      [*]*    *[*]     *[*]
								  *
								  * 
	------------------------------------------------------
	*/
	int i, j; /*i, j  represent the coordinate of the center block in block grid*/
} Piece;

Piece gPiece = { -1,-1,-1,-1 };
Piece gPendingPieces[PIECE_TYPE_CNT];
int gPieceNext = PIECE_TYPE_CNT;
/** TODO: Fill up the declaration of functions**/
// Declaration of functions
int sendEvent(int, Piece*);


// Declaration of Piece related functions
int Piece_Deconstruct(Piece*);
int Piece_Display(Piece*, LTexture*);
int Piece_inValid(Piece*);
int Piece_MoveDown(Piece*);
int Piece_Pan(Piece*, int);
int Piece_Rotate(Piece*);
int Piece_Squeeze(Piece*);

// Piece clearing and displaying
int Piece_inValid(Piece* p) {
	// Return value 0 if p is valid
	// Return value 1 if p exceed edge
	// Return value 2 if p hit bottom or hit other blocks
	for (int i = 0; i < 4; i++) {
		// Manage rotation
		int dx = delta_x[p->type][p->angle][i], dy = delta_y[p->type][p->angle][i];		
		int block_i = p->i + dx, block_j = p->j + dy;
		if (block_i < 0 || block_j < 0) {
			return 1;
		}
		if (block_i >= BLOCKS_IN_WIDTH) {
			return 1;
		}
		if (block_j >= BLOCKS_IN_HEIGHT + BLOCKS_GRID_FIRSTLINE) {
			return 2;
		}
		if (blockGrid[block_j][block_i].texture != NULL) {
			return 2;
		}
	}
	return 0;
}

int Piece_Display(Piece* p, LTexture* texture) {
	for (int i = 0; i < 4; i++) {
		int dx = delta_x[p->type][p->angle][i], dy = delta_y[p->type][p->angle][i];
		int block_i = p->i + dx, block_j = p->j + dy;
		renderLTexture(gRenderer, texture, blockGrid[block_j][block_i].x, blockGrid[block_j][block_i].y);
	}
	return 1;
}

// Piece moving functions
int Piece_MoveDown(Piece* p) {
	Piece newP = *p;
	newP.j = (p->j + 1);
	switch (Piece_inValid(&newP)) {
		case 1:
			return 1;
			break;
		case 2:
			return 2;
			break;
		default:
			*p = newP;
			break;
	}
	return 0;
}

int Piece_Pan(Piece* p, int to_left) {
	Piece newP = *p;
	newP.i = to_left ? (p->i - 1) : (p->i + 1);
	if (Piece_inValid(&newP)) return 0;
	*p = newP;
	return 1;
}

int Piece_Rotate(Piece* p) {
	Piece newP = *p;
 	newP.angle = (p->angle + 1) % 4;
	int type = Piece_inValid(&newP);
	if (type == 2) {
		return 0;
	}
	if (type == 1) {
		Piece_Squeeze(&newP);
	}
	*p = newP;
	return 1;
}

// Deconstruct piece and generating new piece
int Piece_Generate() {
	if (gPieceNext >= PIECE_TYPE_CNT) {
		// Regenerate the sequence
		int typeSequence[PIECE_TYPE_CNT];
		for (int i = 0; i < 7; i++) {
			typeSequence[i] = i;
			swap(&typeSequence[i], &typeSequence[rand() % (i+1)]);
		}
		for (int i = 0; i < 7; i++) {
			gPendingPieces[i].i = rand() % BLOCKS_IN_WIDTH;
			gPendingPieces[i].j = BLOCKS_GRID_FIRSTLINE;
			gPendingPieces[i].angle = rand() % 4;
			gPendingPieces[i].type = typeSequence[i];
			Piece_Squeeze(&gPendingPieces[i]);
		}
		gPieceNext = 0;
	}	
	return 1;
}

int Piece_Squeeze(Piece* p) {
	// Pass a out-of-bound piece to this function and Squeeze it back to game area
	
	// 1.Find which block exceed the boundary, which side of the margin it exceeded
	// and how many blocks away from valid area.
	int exceeded[4] = { 0 }, left_margin = -1, maxE = 0;
	for (int i = 0; i < 4; i++) {
		int dx = delta_x[p->type][p->angle][i], dy = delta_y[p->type][p->angle][i];		
		int block_i = p->i + dx, block_j = p->j + dy;
		if (block_i >= BLOCKS_IN_WIDTH) {
			exceeded[i] = 1;
			left_margin = 0;
			maxE = MAX(maxE, block_i - BLOCKS_IN_WIDTH + 1);
			continue;
		}
		if (block_i < 0) {
			exceeded[i] = 1;
			left_margin = 1;
			maxE = MAX(maxE, -block_i);
			continue;
		}
	}
	if (maxE == 0) {
		return 1;
	}
	if (left_margin) 
		p->i += maxE;
	else
		p->i -= maxE;
	return 1;
}

int Piece_Deconstruct(Piece* p) {
	for (int i = 0; i < 4; i++) {
		int dx = delta_x[p->type][p->angle][i], dy = delta_y[p->type][p->angle][i];
		int block_i = p->i + dx, block_j = p->j + dy;
		blockGrid[block_j][block_i].texture = gBlocks[p->type];
	}
	return 1;
}




/* Definitions of the user events*/
Uint32 gUserEvent = 0;
/* User event code 0: Touched other block or touched the ground*/
int sendEvent(int code, Piece* data) {
	SDL_Event e;
	e.type = gUserEvent;
	e.user.code = code;
	e.user.data1 = data;
	SDL_PushEvent(&e);
	return 1;
}


int init() {
	// Initilize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL initiate error! SDL_Error: %s", SDL_GetError());
		return 0;
	}

	// Create window
	gWindow = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		printf("Could not create window! SDL_Error: %s", SDL_GetError());
		return 0;
	}

	// Create renderer
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == NULL) {
		printf("Could not create renderer! SDL_Error: %s", SDL_GetError());
		return 0;
	}
	// Set drawing mode to alpha blend
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

	// Initilize PNG loading
	int imgFlags = IMG_INIT_PNG; /*Image type which will be used*/
	if (!(IMG_Init(IMG_INIT_PNG) & imgFlags)) {
		printf("Could not initilize PNG image loading! IMG_Error:%s", IMG_GetError());
		return 0;
	}

	// Initialize SDL_ttf
	if (TTF_Init() == -1) {
		printf("Could not initilize TTF! SDL_ttf error:%s", TTF_GetError());
	}

	// Initialize gTimer
	gTimer = Timer_New();

	// Registrating self-created event
	gUserEvent = SDL_RegisterEvents(1);
	if (gUserEvent == (Uint32)-1) {
		printf("Not enough user event left!\n");
		return 0;
	}

	// Initializing random number
	srand(SDL_GetTicks());

	

	return 1;
}

void initBlockGrid() {
	// Initializing block grid
	for (int j = 0, block_y = BLOCK_PTSIZE / 2 - BLOCKS_GRID_FIRSTLINE * BLOCK_PTSIZE; j < BLOCKS_IN_HEIGHT + BLOCKS_GRID_FIRSTLINE; j++, block_y += BLOCK_PTSIZE) {
		for (int i = 0, block_x = SCREEN_CENTER_X - AREA_GAME_WIDTH / 2 + BLOCK_PTSIZE / 2; i < BLOCKS_IN_WIDTH; i++, block_x += BLOCK_PTSIZE) {
			blockGrid[j][i].x = block_x;
			blockGrid[j][i].y = block_y;
			blockGrid[j][i].texture = NULL;
		}
	}

}

int loadMedia() {
	// Load font from file
	gFont = TTF_OpenFont("resource/font/Britanic.ttf", FONT_PTSIZE);
	gInputFont = TTF_OpenFont("resource/font/lazy.ttf", FONT_PTSIZE);

	// Allocate memory
	for (int i = 0; i < PIECE_TYPE_CNT; i++) {
		gBlocks[i] = newLTexture();
	}
	gDropBlock = newLTexture();
	gScore = newLTexture();
	gTitleTexture = newLTexture();
	gBackgroundTexture = newLTexture();
	gRankTexture = newLTexture();

	// Load media file 
	for (int i = 0; i < PIECE_TYPE_CNT; i++) {
		char path[30];
		snprintf(path, 30, "resource/blocks/%s.png", blocksResourceName[i]);
		loadTexture(gBlocks[i], path, gRenderer);
	}
	loadTexture(gDropBlock, "resource/blocks/drop.png", gRenderer);
	loadTexture(gTitleTexture, "resource/title.png", gRenderer);
	loadTexture(gBackgroundTexture, "resource/background.png", gRenderer);
	return 1;
}

int renderBackground() {
	// Clear the screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(gRenderer, &gGameRect);
	SDL_RenderFillRect(gRenderer, &gScoreRect);
	SDL_RenderFillRect(gRenderer, &gNextBlockRect);
	renderLTexture(gRenderer, gBackgroundTexture, SCREEN_CENTER_X, SCREEN_CENTER_Y);
	return 1;
}

int renderGameArea() {
	// Display blocks except those in pieces
	for (int j = BLOCKS_GRID_FIRSTLINE; j < BLOCKS_IN_HEIGHT + BLOCKS_GRID_FIRSTLINE; j++) {
		for (int i = 0; i < BLOCKS_IN_WIDTH; i++) {
			if (blockGrid[j][i].texture == NULL) continue;
			renderLTexture(gRenderer, blockGrid[j][i].texture, blockGrid[j][i].x, blockGrid[j][i].y);
		}
	}

	// Display blocks in gPiece
	if (gPiece.angle == -1) {
		return 0;
	}
	Piece_Display(&gPiece, gBlocks[gPiece.type]);
	return 1;
}

int renderRank() {
	SDL_Color textColor = { 0x20, 0x20, 0x30, 0xFF};
	char rankText[MAX_RANK_LEN * 5];
	memset(rankText, 0, sizeof(rankText));
	for (int i = 0; i < 5; i++) {
		char temp[MAX_RANK_LEN];
		snprintf(temp, MAX_RANK_LEN, "%-6d%7s\n\n\n", gRankScore[i], gRankPlayer[i]);
		strncat(rankText, temp, MAX_RANK_LEN);
	}

	// Render rank 
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(gFont, rankText, textColor, FONT_PTSIZE * (MAX_USERNAME + 2));
	gRankTexture->mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	gRankTexture->h = textSurface->h; gRankTexture->w = textSurface->w;
	SDL_FreeSurface(textSurface);

	renderLTexture(gRenderer, gRankTexture, AREA_RANK_CENTER_X, AREA_RANK_CENTER_Y);
	emptyLTexture(gRankTexture);
	return 1;
}

int renderScore() {
	SDL_Color textColor = { 0x20, 0x20, 0x30, 0xFF };
	char scoreText[100];
	snprintf(scoreText, 100, "%d", gScoreNum);
	loadFromRenderedText(gScore, scoreText, gRenderer, gFont, textColor);
	renderLTexture(gRenderer, gScore, AREA_SCORE_CENTER_X, AREA_SCORE_CENTER_Y + 20);
	return 1;
}

int renderNextPiece() {
	if (gPendingPieces[gPieceNext].angle == -1) return 0;
	for (int i = 0; i < 4; i++) {
		int dx = delta_x[gPendingPieces[gPieceNext].type][0][i];
		int dy = delta_y[gPendingPieces[gPieceNext].type][0][i];
		renderLTexture(gRenderer, gBlocks[gPendingPieces[gPieceNext].type], AREA_NEXTBLOCK_CENTER_X + dx * BLOCK_PTSIZE, AREA_NEXTBLOCK_CENTER_Y + dy * BLOCK_PTSIZE + 100);
	}
	return 1;
}

int renderDropPlace() {
	Piece p = gPiece;
	while (!Piece_MoveDown(&p));
	Piece_Display(&p, gDropBlock);
	return 1;
}

int startGame() {
	tickInterval = 300;
	level = 1, levelDiff = 100, fastFowardInterval = 60, normalInterval = tickInterval;
	gScoreNum = 0;
	dead = 0;
	initBlockGrid();

	// Open rank date for read and write
	gRankFile = fopen("rank.data", "r");

	// Read rank and prepare for render
	for (int i = 0; i < 5; i++) {
		fscanf(gRankFile, "%s %d", &gRankPlayer[i], &gRankScore[i]);
	}
	fclose(gRankFile);
	gRankFile = NULL;

	// Start game
	gPiece.i = 7; gPiece.j = BLOCKS_GRID_FIRSTLINE; gPiece.type = 3; gPiece.angle = 0;
	gPieceNext = PIECE_TYPE_CNT;
	Piece_Generate();
	Timer_Start(gTimer);
	gGameState = 2;
	return 1;
}

int pauseGame() {
	Timer_Pause(gTimer);
	gGameState = 1;
	return 1;
}

int resumeGame() {
	Timer_Resume(gTimer);
	gGameState = 2;
	return 1;
}

int updatePieceDrop(Piece* p, Uint32* lastMoveTime, Uint32 tickInterval) {
	Uint32 nowTime = Timer_GetTime(gTimer);
	if ((nowTime - *lastMoveTime) > tickInterval) {
		*lastMoveTime = nowTime;
		sendEvent(0, p);
	}
	return 1;
}

int clearFullLine() {
	int clearedLine = 0;
	for (int j = BLOCKS_GRID_FIRSTLINE+BLOCKS_IN_HEIGHT-1; j >= BLOCKS_GRID_FIRSTLINE; j--) {
		int full = 1;
		for (int i = 0; i < BLOCKS_IN_WIDTH; i++) {
			if (blockGrid[j][i].texture == NULL) {
				full = 0;
				break;
			}
		}
		if (full) {
			clearedLine++;
			// Add score
			gScoreNum += 100 * clearedLine;
			// If this line is full, then chear
			for (int i = 0; i < BLOCKS_IN_WIDTH; i++) {
				blockGrid[j][i].texture = NULL;
			}
			for (int line = j; line > 0; line--) {
				for(int i = 0; i < BLOCKS_IN_WIDTH; i++){
					blockGrid[line][i].texture = blockGrid[line - 1][i].texture;
				}
			}
			j++;
		}
	}
	return 1;
}


/* Functions in processing stage */
int page_startup();
int page_gaming();
int page_dead();
/*-------------------------------*/

int page_startup() {
	// Return value 1 if user wants to exit, return 0 if not.
	// Load promptText
	LTexture* promptTextTexture = newLTexture();
	SDL_Color textColor = { 0xFA, 0xFA, 0x20, 0xFF};
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(gFont, "  Welcome to Tetris game!\n Press Enter To Start Game\n", textColor, AREA_GAME_WIDTH);
	promptTextTexture->mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	promptTextTexture->h = textSurface->h; promptTextTexture->w = textSurface->w;
	SDL_FreeSurface(textSurface);
	startGame(); // Start background playing game in start up menu
	
	// Declare a texture to handle player name input
	LTexture* nameTexture = newLTexture();

	// quit state
	int quit = 0;
	Uint32 lastMove = 0; /* Used to delay sometime between every random action */
	SDL_Event e;
	SDL_StartTextInput();
	char inputText[20];
	memset(inputText, 0, sizeof(inputText));
	while (!quit) {
		int renderText = 0;
		// Update piece drop
		if (!updatePieceDrop(&gPiece, &lastMoveTime, 100)) {
			printf("Could not update the state of the block: %s!", SDL_GetError());
			return 1;
		}
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
				return 1;
			}
			else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_RETURN) {
					strcpy(gCurrentPlayer, strlen(inputText) ? inputText : "noname");
					startGame();
					quit = 1;
					return 0;
				}
			 //Handle backspace
				else if( e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0 )
				{
					//lop off character
					inputText[strlen(inputText) - 1] = '\0';
					renderText = 1;
				}
			}
			else if (e.type == SDL_TEXTINPUT) {
				if (strlen(inputText) > MAX_USERNAME) continue;

				strcat(inputText, e.text.text );
				renderText = 1;
			}
			else if (e.type == gUserEvent) {
				if (e.user.code == 0) {
					switch (Piece_MoveDown(&gPiece)) {
						case 2:
							sendEvent(1, 0);
							break;
						default:
							break;
					}
				}
				else if (e.user.code == 1) {
					// Piece touches ground
					Piece_Deconstruct(&gPiece);
					clearFullLine();
					// Check if game is over
					for (int i = 0; i < BLOCKS_IN_WIDTH; i++) {
						if (blockGrid[BLOCKS_GRID_FIRSTLINE - 1][i].texture != NULL) {
							dead = 1;
							SDL_Delay(3000);
							startGame();
							break;
						}
					}
					gPiece = gPendingPieces[gPieceNext++];
					Piece_Generate();
				}
			}
		}
		

		// Randomly do things
		Uint32 timeNow = Timer_GetTime(gTimer);
		Uint32 randTime = rand() % 2000 + 300;
		if ((timeNow - lastMove)> randTime){
			switch (rand() % 4) {
				case 0:
					Piece_Rotate(&gPiece); break;
				case 1:
					Piece_Pan(&gPiece, 0); break;
				case 2:
					Piece_Pan(&gPiece, 1); break;
			}
			lastMove = timeNow;
		}

		// Clear Screen
		SDL_SetRenderDrawColor(gRenderer, 0x20, 0x20, 0x30, 0x50);
		SDL_RenderClear(gRenderer);

		renderGameArea();
		SDL_RenderFillRect(gRenderer, &gScreenRect);
		renderLTexture(gRenderer, gTitleTexture, SCREEN_CENTER_X, SCREEN_CENTER_Y);
		renderLTexture(gRenderer, promptTextTexture, SCREEN_CENTER_X + 40, SCREEN_CENTER_Y + 150);
		
		SDL_Color inputTextColor = { 0xFF,0xFF,0xFF,0xFF };
		//Render input text if needed
		if (strlen(inputText) > 0) {
			loadFromRenderedText(nameTexture, inputText, gRenderer, gInputFont, inputTextColor);
		}
		else {
			loadFromRenderedText(nameTexture, "Your Name:", gRenderer, gInputFont, inputTextColor);
		}
		renderLTexture(gRenderer, nameTexture, SCREEN_CENTER_X, SCREEN_CENTER_Y + 150 + FONT_PTSIZE + 40);

		SDL_RenderPresent(gRenderer);
	}
	SDL_StopTextInput();
	emptyLTexture(promptTextTexture);
	return 0;
}

int page_gaming() {
	// Initialize game scene
	if (!renderBackground()) {
		printf("Could not initialize the game scene! SDL_Error: %s", SDL_GetError());
		return 0;
	}

	SDL_Event e;
	int quit = 0;

	while(!quit && !dead){
		// Runing game
		// Update self implement events
		if (!updatePieceDrop(&gPiece, &lastMoveTime, tickInterval)) {
			printf("Could not update the state of the block: %s!", SDL_GetError());
			return 1;
		}
		
		// Update tick interval according to score
		while (gScoreNum / (levelDiff*level) > level && level < 8) {
			normalInterval = normalInterval - normalInterval/10;
			tickInterval = normalInterval;
			level++;
		}

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}
			else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					quit = 1;
				}
				if (e.key.keysym.sym == SDLK_w) {
					if (gGameState == 2)
						Piece_Rotate(&gPiece);
				}
				if (e.key.keysym.sym == SDLK_a) {
					if (gGameState == 2)
						Piece_Pan(&gPiece, 1);
				}
				if (e.key.keysym.sym == SDLK_d) {
					if (gGameState == 2)
						Piece_Pan(&gPiece, 0);
				}
				if (e.key.keysym.sym == SDLK_s) {
					if (gGameState == 2)
						tickInterval = fastFowardInterval;
				}
				if (e.key.keysym.sym == SDLK_SPACE) {
					if (gGameState == 1) /* Only paused game could be started */
						resumeGame();
					else if (gGameState == 2) /* Only running game could be paused */
						pauseGame();
				}
			}
			else if (e.type == SDL_KEYUP) {
				if (e.key.keysym.sym == SDLK_s) {
					tickInterval = normalInterval;
				}
			}
			else if (e.type == gUserEvent) {
				if (e.user.code == 0) {
					switch (Piece_MoveDown(&gPiece)) {
						case 2:
							sendEvent(1, 0);
							break;
						default:
							break;
					}
				}
				else if (e.user.code == 1) {
					// Piece touches ground
					Piece_Deconstruct(&gPiece);
					clearFullLine();
					// Check if game is over
					for (int i = 0; i < BLOCKS_IN_WIDTH; i++) {
						if (blockGrid[BLOCKS_GRID_FIRSTLINE - 1][i].texture != NULL) {
							dead = 1;
							break;
						}
					}
					gPiece = gPendingPieces[gPieceNext++];
					Piece_Generate();
				}
			}
		}

		if (dead) {
			// Update rank
			int insertPlace = -1;
			for (int i = 0; i < 5; i++) {
				if (gRankScore[i] <= gScoreNum) {
					insertPlace = i;
					break;
				}
			}
			for (int i = insertPlace; i < 4; i++) {
				gRankScore[i + 1] = gRankScore[i];
				strcpy(gRankPlayer[i + 1], gRankPlayer[i]);
			}
			gRankScore[insertPlace] = gScoreNum;
			strcpy(gRankPlayer[insertPlace], gCurrentPlayer);
			gRankFile = fopen("rank.data", "w+");
			// update data to file
			for (int i = 0; i < 5; i++) {
				fprintf(gRankFile, "%s %d ", gRankPlayer[i], gRankScore[i]);
			}
			fclose(gRankFile);
			gRankFile = NULL;

			// go to dead scene
			if (page_dead()) {
				startGame();
			}
			else
				return 1;
		}

		// Clear the screen
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);

		// Render background
		renderBackground();
			
		// Render game area
		renderGameArea();

		// Render rank
		renderRank();

		// Render promptText
		LTexture* promptTextTexture = newLTexture();
		SDL_Color textColor = { 0x20, 0xFF, 0xAA, 0xFF};
		if (gGameState == 1) {
			SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(gFont, "Game Paused", textColor, AREA_GAME_WIDTH);
			promptTextTexture->mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
			promptTextTexture->h = textSurface->h; promptTextTexture->w = textSurface->w;
			SDL_FreeSurface(textSurface);
			renderLTexture(gRenderer, promptTextTexture, SCREEN_CENTER_X, SCREEN_CENTER_Y);

			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x60);
			SDL_RenderFillRect(gRenderer, &gScreenRect);

			SDL_RenderPresent(gRenderer);
			emptyLTexture(promptTextTexture);
			continue;
		}

		// Render scoreboard
		renderScore();

		// Render next piece area
		renderNextPiece();

		// Render where the piece will stop
		renderDropPlace();
		
		SDL_RenderPresent(gRenderer);
	}
	return 1;
}
int page_dead() {
	// return 1 to restart, return 0 to exit
	LTexture* promptTextTexture = newLTexture();
	int a = 0x00;
	SDL_Event e;

	SDL_SetRenderDrawColor(gRenderer, 0x20, 0x20, 0x20, 0xA0);
	SDL_RenderFillRect(gRenderer, &gScreenRect);

	SDL_Color textColor = { 0x20, 0xFF, 0xAA, 0xFF};
	char gameoverText[200];
	snprintf(gameoverText, 200, "Game Over!\nYour Score: %d\nPress enter to restart!", gScoreNum);
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(gFont, gameoverText, textColor, AREA_GAME_WIDTH - 50);
	promptTextTexture->mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	promptTextTexture->h = textSurface->h; promptTextTexture->w = textSurface->w;
	SDL_FreeSurface(textSurface);
	renderLTexture(gRenderer, promptTextTexture, SCREEN_CENTER_X, SCREEN_CENTER_Y);
	SDL_RenderPresent(gRenderer);
	emptyLTexture(promptTextTexture);
	int quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_RETURN) {
					return 1;
				}
			}
		}
	}
	return 0;
 }

void close() {
    //Free global font
    TTF_CloseFont( gFont );
    gFont = NULL;

	TTF_CloseFont(gInputFont);
	gFont = NULL;

    //Destroy window    
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;
	
    //Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char *args[]) {
	// Initialize
	if (!init()) {
		printf("Could not initilize! SDL_Error: %s!", SDL_GetError());
		return 0;
	}
	
	// LoadMedia
	if (!loadMedia()) {
		printf("Could not load media! SDL_Error: %s!", SDL_GetError());
		return 0;
	}

	if (page_startup()) {
		return 0;
	}
	if (page_gaming()) {
		return 0;
	}

	close();

	return 0;
}