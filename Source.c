#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>
#include "LTexture.h"
#include "Timer.h"
#include "Block.h"

void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}
/* Info about the screen*/
#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720 
#define SCREEN_CENTER_X (SCREEN_WIDTH/2)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT/2)

#define AREA_GAME_WIDTH 450
#define AREA_SCORE_WIDTH 200
#define AREA_SCORE_HEIGHT 150
#define AREA_SCORE_CENTER_X (SCREEN_CENTER_X + AREA_GAME_WIDTH/2 + 200)
#define AREA_SCORE_CENTER_Y (150)
#define AREA_NEXTBLOCK_WIDTH 150
#define AREA_NEXTBLOCK_HEIGHT 150
#define AREA_NEXTBLOCK_CENTER_X (SCREEN_CENTER_X + AREA_GAME_WIDTH/2 + 200) 
#define AREA_NEXTBLOCK_CENTER_Y (400)
/************************/

/* Info about the block grid*/
#define BLOCK_PTSIZE 30
#define BLOCKS_IN_WIDTH (AREA_GAME_WIDTH/BLOCK_PTSIZE)
#define BLOCKS_IN_HEIGHT (SCREEN_HEIGHT/BLOCK_PTSIZE)

/* Devide the screen into several rectangles */
SDL_Rect gGameRect = { SCREEN_CENTER_X-AREA_GAME_WIDTH/2, 0, AREA_GAME_WIDTH,SCREEN_HEIGHT };
SDL_Rect gScoreRect = { AREA_SCORE_CENTER_X - AREA_SCORE_WIDTH / 2, AREA_SCORE_CENTER_Y - AREA_SCORE_HEIGHT / 2,
						AREA_SCORE_WIDTH, AREA_SCORE_HEIGHT };
SDL_Rect gNextBlockRect = { AREA_NEXTBLOCK_CENTER_X - AREA_NEXTBLOCK_WIDTH / 2, AREA_NEXTBLOCK_CENTER_Y - AREA_NEXTBLOCK_HEIGHT / 2,
						AREA_NEXTBLOCK_WIDTH, AREA_SCORE_HEIGHT };
// Define single block as a struct and group them together before touching the ground
#define BLOCKS_IN_A_GROUP 4

/* Info about the font */
#define FONT_PTSIZE 30
/***********************/


// Global variables for window
SDL_Window* gWindow = NULL; /*window to display things on*/
SDL_Renderer* gRenderer = NULL; /*The renderer will be used*/
Timer* gTimer = NULL; /*Timer is used to manage time in game*/
LTexture* gBlock = NULL;
TTF_Font* gFont = NULL;
LTexture* gTimeTextTexture = NULL;


Block blockGrid[BLOCKS_IN_WIDTH][BLOCKS_IN_HEIGHT] = { 0 };


/* Define pieces in this section*/
/* Select a central block in each type of piece,
   define other blocks using the delta value */
#define PIECE_TYPE_CNT 6
// int delta_x[PIECE_TYPE_CNT][4] = { { 0, 1, 0,-1},{ 0, 1,-1,-1},{ 0, 1, 1,-1},{ 0, 0, 0, 0},{ 0, 1, 0,-1},{ 0, 1, 0,-1} };
// int delta_y[PIECE_TYPE_CNT][4] = { { 0, 0,-1, 0},{ 0, 0,-1, 0},{ 0, 0,-1, 0},{ 0, 1,-1,-2},{ 0, 0,-1,-1},{ 0,-1,-1, 0} };

int delta_x[PIECE_TYPE_CNT][4][4] = {
	{{ 0, 1, 0,-1},{ 0, 0,-1, 0},{ 0,-1, 0, 1},{ 0, 0, 1, 0}},
	{{ 0, 1,-1,-1},{ 0, 0,-1, 0},{ 0,-1, 1, 1},{ 0, 0, 1, 0}},
	{{ 0, 1, 1,-1},{ 0, 0,-1, 0},{ 0,-1,-1, 1},{ 0, 0, 1, 0}},
	{{ 0, 0, 0, 0},{ 0, 1,-1,-2},{ 0, 0, 0, 0},{ 0,-1, 1, 2}},
	{{ 0, 1, 0,-1},{ 0, 0,-1,-1},{ 0,-1, 0, 1},{ 0, 0, 1, 1}},
	{{ 0, 1, 0,-1},{ 0,-1,-1, 0},{ 0,-1, 0, 1},{ 0, 1, 1, 0}}
};
int delta_y[PIECE_TYPE_CNT][4][4] = {
	{{ 0, 0,-1, 0},{ 0,-1, 0, 1},{ 0, 0, 1, 0},{ 0, 1, 0,-1}},
	{{ 0, 0,-1, 0},{ 0,-1, 1, 1},{ 0, 0, 1, 0},{ 0, 1,-1,-1}},
	{{ 0, 0,-1, 0},{ 0,-1,-1, 1},{ 0, 0, 1, 0},{ 0, 1, 1,-1}},
	{{ 0, 1,-1,-2},{ 0, 0, 0, 0},{ 0,-1, 1, 2},{ 0, 0, 0, 0}},
	{{ 0, 0,-1,-1},{ 0,-1, 0, 1},{ 0, 0, 1, 1},{ 0, 1, 0,-1}},
	{{ 0,-1,-1, 0},{ 0,-1, 0, 1},{ 0, 1, 1, 0},{ 0, 1, 0,-1}}
};

/*4 indicates that there are 4 blocks in each piece */

typedef struct {
	int type, angle;
	/*
	------------------ TYPE DESCRIPTION ------------------ 
	Type 0:  Type 1:  Type 2:  Type 3:  Type 4:  Type 5:
	   *      *        *          *      * *        * *
	 *[*]*    *[*]*    *[*]*     [*]      [*]*    *[*]
								  *
								  * 
	------------------------------------------------------
	*/
	int i, j; /*i, j  represent the coordinate of the center block in block grid*/
} Piece;

Piece gPiece = { 1, 0, 5, 5 };
/** TODO: Fill up the declaration of functions**/
// Declaration of functions
int sendEvent(int, Piece*);

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
			printf("Piece exceed edge!\n");
			return 1;
		}
		if (block_i >= BLOCKS_IN_WIDTH) {
			printf("Piece exceed edge!\n");
			return 1;
		}
		if (block_j >= BLOCKS_IN_HEIGHT) {
			printf("Hit base\n");
			return 2;
		}
		if (blockGrid[block_i][block_j].texture != NULL) {
			// Check if this non NULL block belongs to gPiece
			// If so, check the next block
			int in_gPiece = 0;
			for (int j = 0; j < 4; j++) {
				if (gPiece.i + delta_x[p->type][p->angle][j] == block_i && gPiece.j + delta_y[p->type][p->angle][j] == block_j) {
					in_gPiece = 1;
					break;
				}
			}
			if (in_gPiece) continue;
			else {
				return 2;
			}
		}
	}
	return 0;
}
int Piece_Clear(Piece* p) {
	if (p == NULL) {
		printf("Could not remove piece because it's empty!\n");
		return 0;
	}
	for (int i = 0; i < 4; i++) {
		int dx = delta_x[p->type][p->angle][i], dy = delta_y[p->type][p->angle][i];		
		int block_i = p->i + dx, block_j = p->j + dy;
		blockGrid[block_i][block_j].texture = NULL;
	}
	return 1;
}

int Piece_Display(Piece* p) {
	for (int i = 0; i < 4; i++) {
		int dx = delta_x[p->type][p->angle][i], dy = delta_y[p->type][p->angle][i];		
		int block_i = p->i + dx, block_j = p->j + dy;
		blockGrid[block_i][block_j].texture = gBlock;
	}
	return 1;
}

// Piece moving functions
int Piece_MoveDown(Piece* p) {
	Piece newP = *p;
	newP.j = (p->j + 1);
	switch (Piece_inValid(&newP)) {
		case 1:
			return 0;
			break;
		case 2:
			sendEvent(1, 0);
			return 0;
			break;
		default:
			if (!Piece_Clear(p)) return 0;
			*p = newP;
			if (!Piece_Display(p)) return 0;
			break;
	}
	return 1;
}

int Piece_Pan(Piece* p, int to_left) {
	Piece newP = *p;
	newP.i = to_left ? (p->i - 1) : (p->i + 1);
	if (Piece_inValid(&newP)) return 0;
	if (!Piece_Clear(p)) return 0;
	*p = newP;
	if (!Piece_Display(p)) return 0;
	return 1;
}

int Piece_Rotate(Piece* p) {
	Piece newP = *p;
 	newP.angle = (p->angle + 1) % 4;
	if (Piece_inValid(&newP)) return 0;
	if (!Piece_Clear(p)) return 0;
	*p = newP;
	if (!Piece_Display(p)) return 0;
	return 1;
}

// Deconstruct piece and generating new piece
int Piece_Generate(Piece* p) {
	if (p == NULL) return 0;
	p->i = rand() % BLOCKS_IN_WIDTH;
	p->j = 0;
	p->angle = rand() % 4;
	p->type = rand() % PIECE_TYPE_CNT;
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
	
	// Initializing block grid
	for (int i = 0, block_x = SCREEN_CENTER_X - AREA_GAME_WIDTH/2 + BLOCK_PTSIZE/2; i < BLOCKS_IN_WIDTH; i++, block_x += BLOCK_PTSIZE) {
		for (int j = 0, block_y = BLOCK_PTSIZE/2; j < BLOCKS_IN_HEIGHT; j++, block_y += BLOCK_PTSIZE) {
			blockGrid[i][j].x = block_x;
			blockGrid[i][j].y = block_y;
			blockGrid[i][j].texture = NULL;
		}
	}

	return 1;
}

int loadMedia() {
	// Load font from file
	gFont = TTF_OpenFont("resource/font/Consolas.ttf", FONT_PTSIZE);

	gBlock = newLTexture();
	gTimeTextTexture = newLTexture();

	// Load media file 
	loadTexture(gBlock, "resource/block.png", gRenderer);
	
	return 1;
}

int renderBackground() {
	// Clear the screen
	SDL_SetRenderDrawColor(gRenderer,0xFF,0xFF,0xFF,0xFF);
	SDL_RenderFillRect(gRenderer, &gGameRect);
	SDL_RenderFillRect(gRenderer, &gScoreRect);
	SDL_RenderFillRect(gRenderer, &gNextBlockRect);
	return 1;
}

int renderGameArea() {
	for (int i = 0; i < BLOCKS_IN_WIDTH; i++) {
		for (int j = 0; j < BLOCKS_IN_HEIGHT; j++) {
			if (blockGrid[i][j].texture == NULL) continue;
			renderLTexture(gRenderer, blockGrid[i][j].texture, blockGrid[i][j].x, blockGrid[i][j].y);
		}
	}
	return 1;
}

int startGame() {
	// Start game
	Timer_Start(gTimer);
	return 1;
}

int pauseGame() {
	Timer_Pause(gTimer);
	return 1;
}

int resumeGame() {
	Timer_Resume(gTimer);
	return 1;
}

int updatePieceDrop(Piece* p, Uint32* lastMoveTime) {
	Uint32 nowTime = Timer_GetTime(gTimer);
	if (nowTime - *lastMoveTime > 300) {
		*lastMoveTime = nowTime;
		sendEvent(0, p);
	}
	return 1;
}

void close() {
    //Free global font
    TTF_CloseFont( gFont );
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

	// Initialize game scene
	if (!renderBackground()) {
		printf("Could not initialize the game scene! SDL_Error: %s", SDL_GetError());
		return 0;
	}

	int quit = 0; /*set quit to 1 if user press 'x' of the window*/
	SDL_Event e;
	Uint32 lastMoveTime = 0;
	// Mainloop
	while (!quit) {
		// Update self implement events
		if (!updatePieceDrop(&gPiece, &lastMoveTime)) {
			printf("Could not update the state of the block: %s!", SDL_GetError());
			return 1;
		}
			
		// Deal with events in queue
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}
			else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					quit = 1;
				}
				if (e.key.keysym.sym == SDLK_RETURN) {
					startGame();
				}
				if (e.key.keysym.sym == SDLK_w) {
					Piece_Rotate(&gPiece);
				}
				if (e.key.keysym.sym == SDLK_a) {
					Piece_Pan(&gPiece, 1);
				}
				if (e.key.keysym.sym == SDLK_d) {
					Piece_Pan(&gPiece, 0);
				}
				if (e.key.keysym.sym == SDLK_SPACE) {
					if (gTimer->paused)
						resumeGame();
					else
						pauseGame();
				}
			}
			else if (e.type == gUserEvent) {
				if (e.user.code == 0){
					Piece_MoveDown(&gPiece);
				}
				else if (e.user.code == 1) {
					Piece_Generate(&gPiece);
				}
			}
		}
		Piece_Display(&gPiece);
		// Clear the screen
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);

		// Render background
		renderBackground();
		renderGameArea();
		
		// Render time text
		SDL_Color textColor = { 0x50, 0xAF, 0x50, 0xFF };
		loadFromRenderedText(gTimeTextTexture,Timer_GetStr(gTimer), gRenderer, gFont, textColor);

		renderLTexture(gRenderer, gTimeTextTexture, SCREEN_CENTER_X, SCREEN_CENTER_Y);
		SDL_RenderPresent(gRenderer);
	}

	close();

	return 0;
}