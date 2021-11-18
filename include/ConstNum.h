#ifndef H_TETRIS_CONSTNUM
#define H_TETRIS_CONSTNUM
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
#define AREA_NEXTBLOCK_WIDTH 200 
#define AREA_NEXTBLOCK_HEIGHT 200 
#define AREA_NEXTBLOCK_CENTER_X (SCREEN_CENTER_X + AREA_GAME_WIDTH/2 + 200) 
#define AREA_NEXTBLOCK_CENTER_Y (400)
#define AREA_RANK_CENTER_X 200
#define AREA_RANK_CENTER_Y SCREEN_CENTER_Y
/************************/

/* Info about the block grid*/
#define BLOCK_PTSIZE 30
#define BLOCKS_IN_WIDTH (AREA_GAME_WIDTH/BLOCK_PTSIZE)
#define BLOCKS_IN_HEIGHT (SCREEN_HEIGHT/BLOCK_PTSIZE)
#define BLOCKS_GRID_FIRSTLINE 4
#define PIECE_TYPE_CNT 7

// Define single block as a struct and group them together before touching the ground
#define BLOCKS_IN_A_GROUP 4

/* Info about the font */
#define FONT_PTSIZE 30
/***********************/

#define MAX_USERNAME 6
#define MAX_RANK_LEN 40

#endif // H_TETRIS_CONSTNUM