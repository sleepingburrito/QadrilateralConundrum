//outside include
#include <CTYPE.H>
#include "System.h"

//my include
#include "TLS.H"
#include "CD.H"
#include "DRW.H"

//background files
#define START_FILE_BG "STBG.TIM"
#define GAME_FILE_BG "BG.TIM"
#define PAUSE_FILE_BG "PAUB.TIM"
#define CREDIT_FILE_BG "CRDB.TIM"

//game screens
enum{
	SCREEN_STATE_START,
	SCREEN_STATE_GAME,
	SCREEN_STATE_PAUSED,
	SCREEN_STATE_CREDITS
};
char gameScreenState;

//user IO
#define HOLD_DOWN_REPEAT_TIME 5
unsigned int keyRepeatTimer; //directions

//board setup
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 24
#define BOARD_MAX (BOARD_WIDTH * BOARD_HEIGHT)
enum{
	BLOCK_I,
	BLOCK_J,
	BLOCK_L,
	BLOCK_O,
	BLOCK_S,
	BLOCK_T,
	BLOCK_Z,
	BLOCK_MAX_COUNT,
	BLOCK_N //non
};
char board[BOARD_MAX];

void BoardSetup(void){
	short i = BOARD_MAX;
	while(--i > -1) board[i] = BLOCK_N;
}

bool RemoveCompletedLine(void){//removes only 1 completed line. returns true if it removed a line
	short y, x, y2;
	for(y = 0; y < BOARD_HEIGHT; ++y){
		//see how many blocks in line
		for(x = 0; x < BOARD_WIDTH && board[y * BOARD_WIDTH + x] != BLOCK_N; ++x);
		//if full of blocks move top lines down
		if (x == BOARD_WIDTH){
			//move top blocks down
			for(y2 = y-1; y2 > 0; --y2) 
				for(x = 0; x < BOARD_WIDTH; ++x) 
					board[(y2 + 1) * BOARD_WIDTH + x] = board[y2 * BOARD_WIDTH + x];
			return true;
		}
	}
	return false;
}

//board graphic settings
#define GRAPHIC_WIDTH 8
#define GRAPHIC_HEIGHT GRAPHIC_WIDTH
#define BLOCK_DEPTH 250
#define GRAPHIC_X_OFFSET 69
#define GRAPHIC_Y_OFFSET 28
GsIMAGE * blockSpriteSheet;
GsSPRITE blocksGraphic[BLOCK_MAX_COUNT];
FullSprite blockSpriteMetaData;


//falling block data
#define BLOCK_SPRITE_FILE "BLK.TIM"
#define BLOCK_WIDTH 4
#define BLOCK_HEIGHT BLOCK_WIDTH
#define BLOCK_MAX (BLOCK_WIDTH * BLOCK_HEIGHT)
#define BLOCK_ROTATION_COUNT 4
#define BLOCK_FALL_START_X 3
#define BLOCK_FALL_START_Y 0

#define BLOCK_HELD_GRAPHIC_X 180
#define BLOCK_HELD_GRAPHIC_Y 55
#define BLOCK_NEXT_GRAPHIC_X 180
#define BLOCK_NEXT_GRAPHIC_Y 145

const char * currentFallingBlock;
short blockFallingX, blockFallingY;
char blockFallingRotation, fallingBlockId;
char nextFallingId, fallingHoldingId;
bool heldThisBlock, quickFall;
unsigned int fallBlockTimer, fallBlockTimerValue;
unsigned int lineScore;
bool gameOver;

//score
#define SCORE_GRAPHIC_X 18
#define SCORE_GRAPHIC_Y 44
#define LEVEL_GRAPHIC_X 23
#define LEVEL_GRAPHIC_Y 137

#define SCORE_LINE1 4
#define SCORE_LINE2 10
#define SCORE_LINE3 30
#define SCORE_LINE4 120

#define SCORE_SPEED_THRESHOLD 300
#define SCORE_SPEED_START 20
#define SCORE_SPEED_MAX 8

const char blocks[] =
{
	//BLOCK_I
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_I, BLOCK_I, BLOCK_I, BLOCK_I,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_I, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_I, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_I, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_I, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_I, BLOCK_I, BLOCK_I, BLOCK_I,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_I, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_I, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_I, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_I, BLOCK_N, BLOCK_N,
	
	//BLOCK_J
	BLOCK_J, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_J, BLOCK_J, BLOCK_J, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_J, BLOCK_J, BLOCK_N,
	BLOCK_N, BLOCK_J, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_J, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_J, BLOCK_J, BLOCK_J, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_J, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_J, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_J, BLOCK_N, BLOCK_N,
	BLOCK_J, BLOCK_J, BLOCK_N, BLOCK_N,
	
	//BLOCK_L
	BLOCK_N, BLOCK_N, BLOCK_L, BLOCK_N,
	BLOCK_L, BLOCK_L, BLOCK_L, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_L, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_L, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_L, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_L, BLOCK_L, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_L, BLOCK_L, BLOCK_L, BLOCK_N,
	BLOCK_L, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_L, BLOCK_L, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_L, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_L, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	//BLOCK_O
	BLOCK_N, BLOCK_O, BLOCK_O, BLOCK_N,
	BLOCK_N, BLOCK_O, BLOCK_O, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_O, BLOCK_O, BLOCK_N,
	BLOCK_N, BLOCK_O, BLOCK_O, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_O, BLOCK_O, BLOCK_N,
	BLOCK_N, BLOCK_O, BLOCK_O, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_O, BLOCK_O, BLOCK_N,
	BLOCK_N, BLOCK_O, BLOCK_O, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	//BLOCK_S
	BLOCK_N, BLOCK_S, BLOCK_S, BLOCK_N,
	BLOCK_S, BLOCK_S, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_S, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_S, BLOCK_S, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_S, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_S, BLOCK_S, BLOCK_N,
	BLOCK_S, BLOCK_S, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_S, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_S, BLOCK_S, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_S, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	//BLOCK_T
	BLOCK_N, BLOCK_T, BLOCK_N, BLOCK_N,
	BLOCK_T, BLOCK_T, BLOCK_T, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_T, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_T, BLOCK_T, BLOCK_N,
	BLOCK_N, BLOCK_T, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_T, BLOCK_T, BLOCK_T, BLOCK_N,
	BLOCK_N, BLOCK_T, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_T, BLOCK_N, BLOCK_N,
	BLOCK_T, BLOCK_T, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_T, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	//BLOCK_Z
	BLOCK_Z, BLOCK_Z, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_Z, BLOCK_Z, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_Z, BLOCK_N,
	BLOCK_N, BLOCK_Z, BLOCK_Z, BLOCK_N,
	BLOCK_N, BLOCK_Z, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_Z, BLOCK_Z, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_Z, BLOCK_Z, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
	
	BLOCK_N, BLOCK_Z, BLOCK_N, BLOCK_N,
	BLOCK_Z, BLOCK_Z, BLOCK_N, BLOCK_N,
	BLOCK_Z, BLOCK_N, BLOCK_N, BLOCK_N,
	BLOCK_N, BLOCK_N, BLOCK_N, BLOCK_N,
};

char const * GetBlockIndex(const short blockID, const short rot){
	return &blocks[(blockID * BLOCK_MAX * BLOCK_ROTATION_COUNT) + ((rot % BLOCK_ROTATION_COUNT) * BLOCK_MAX)];
}

void UpdateFallingBlockIDRotation(){//call this when blockFallingRotation or fallingBlockId has been updated
	currentFallingBlock = GetBlockIndex(fallingBlockId, blockFallingRotation);
}

bool CheckFallingBlockCollision(void){
	short x, y, xOff, yOff;
	for(y = 0; y < BLOCK_HEIGHT; y++){
		yOff = y + blockFallingY;
		for(x = 0; x < BLOCK_WIDTH; x++){
			xOff = x + blockFallingX;
			if (currentFallingBlock[y * BLOCK_WIDTH + x] != BLOCK_N){
				if (yOff >= BOARD_HEIGHT || xOff < 0 || xOff >= BOARD_WIDTH){ //out of bounds
					return true;
				} else if (board[yOff * BOARD_WIDTH + xOff] != BLOCK_N){ //touching other blocks
					return true;
				}
			}
		}
	}
	return false;
}

void SetFallingBlockXY(const short x, const short y){ //checks collision
	const short backupX = blockFallingX;
	const short backupY = blockFallingY;
	blockFallingX = x;
	blockFallingY = y;
	if (CheckFallingBlockCollision()){
		blockFallingX = backupX;
		blockFallingY = backupY;
	}
}

void SetFallingBlockXYrelative(const short x, const short y){
	SetFallingBlockXY(blockFallingX + x, blockFallingY + y);
}

void SetFallingBlockRotate(const char dir){ //dir is -1 or 1 for left or right rotation
	char i = 0;
	const short startingX = blockFallingX;
	//rotate block
	blockFallingRotation += dir;
	UpdateFallingBlockIDRotation();
	//dance the block left and right 2 spaces to find a free space
	for(; i < 3; ++i){
		blockFallingX = startingX + i;
		if (!CheckFallingBlockCollision()) return;
		blockFallingX = startingX - i;
		if (!CheckFallingBlockCollision()) return;
	};
	//failed to find a free spot, revert all changes
	blockFallingX = startingX;
	blockFallingRotation -= dir;
	UpdateFallingBlockIDRotation();
}

bool CopyToBoardCheckFallFallingBlock(void){ 
	//checks if you can't move down, copys you to board if you cant or moves you down if you can
	//this also removes completed lines and adds to the score
	//returns true if block has hit bottom
	short x, y, xOff, yOff, blockIdTmp, lineCount;
	++blockFallingY;
	if (CheckFallingBlockCollision()){ //check if you can't go down
		--blockFallingY;
		//copy falling pieces to board
		for(y = 0; y < BLOCK_HEIGHT; ++y){
			yOff = y + blockFallingY;
			for(x = 0; x < BLOCK_WIDTH; ++x){
				xOff = x + blockFallingX;
				blockIdTmp = currentFallingBlock[y * BLOCK_WIDTH + x];
				if (blockIdTmp != BLOCK_N){
					board[yOff * BOARD_WIDTH + xOff] = blockIdTmp;
				}
			}
		}
		//remove completed lines and do scoring
		lineCount = 0;
		while(RemoveCompletedLine()) ++lineCount;
		switch(lineCount){
			case 1: lineScore += SCORE_LINE1; break;
			case 2: lineScore += SCORE_LINE2; break;
			case 3: lineScore += SCORE_LINE3; break;
			case 4: lineScore += SCORE_LINE4; break;
		}
		//speed up game based on score
		fallBlockTimerValue = Max(SCORE_SPEED_MAX, SCORE_SPEED_START - (lineScore / SCORE_SPEED_THRESHOLD));
		return true;
	}
	return false;
}

bool GenerateNewFallingBlock(void){ //returns false if cant spawned block is blocked (game over)
	fallingBlockId = nextFallingId;
	nextFallingId = Gen0To6();
	
	blockFallingRotation = 0;
	UpdateFallingBlockIDRotation();
	
	blockFallingX = BLOCK_FALL_START_X;
	blockFallingY = BLOCK_FALL_START_Y;
	
	return !CheckFallingBlockCollision();
}

void HoldBlock(void){
	char tmpId;
	//can only hold once per new block
	if (heldThisBlock) return;
	//if not holding a block take from the next
	if (fallingHoldingId == BLOCK_N){ 
		fallingHoldingId = fallingBlockId;
		fallingBlockId = nextFallingId;
		nextFallingId = Gen0To6();
	}else{ //else swap held block for current
		tmpId = fallingBlockId;
		fallingBlockId = fallingHoldingId;
		fallingHoldingId = tmpId;
	}
	UpdateFallingBlockIDRotation();
	heldThisBlock = true;
	blockFallingX = BLOCK_FALL_START_X;
	blockFallingY = BLOCK_FALL_START_Y;
	fallBlockTimer = TimerSet(fallBlockTimerValue);
}

void NewGame(void){
	BoardSetup();
	nextFallingId = Gen0To6();
	quickFall = false;
	gameOver = false;
	fallingHoldingId = BLOCK_N;
	heldThisBlock = false;
	lineScore = 0;
	keyRepeatTimer = 0;
	fallBlockTimerValue = SCORE_SPEED_START;
	fallBlockTimer = TimerSet(fallBlockTimerValue);
	GenerateNewFallingBlock();
}


//board graphics
void DrawFullBoard(void){ //code for drawing board
	short x, y, yOffset, index, blockId;
	for(y = 0; y < BOARD_HEIGHT; ++y){
		yOffset = y * BOARD_WIDTH;
		for(x = 0; x < BOARD_WIDTH; ++x){
			index = yOffset + x;
			blockId = board[index];
			if (blockId != BLOCK_N){
				blockSpriteMetaData.frame = blockSpriteMetaData.startFrame + blockId;
				blockSpriteMetaData.frame->x = x * GRAPHIC_WIDTH + GRAPHIC_X_OFFSET;
				blockSpriteMetaData.frame->y = y * GRAPHIC_HEIGHT + GRAPHIC_Y_OFFSET;
				GsSortFastSprite(blockSpriteMetaData.frame, ot, BLOCK_DEPTH);
			}
		}
	}
}

void DrawBlock(const char * const blockToDraw, const short xGfxOffset, const short yGfxOffset, const short xOffsetBoard, const short yOffsetBoard){
	short x, y, xOff, yOff, blockId;
	for(y = 0; y < BLOCK_HEIGHT; y++){
		yOff = y + yOffsetBoard;
		for(x = 0; x < BLOCK_WIDTH; x++){
			xOff = x + xOffsetBoard;
			blockId = blockToDraw[y * BLOCK_WIDTH + x];
			if (blockId != BLOCK_N){
				blockSpriteMetaData.frame = blockSpriteMetaData.startFrame + blockId;
				blockSpriteMetaData.frame->x = xOff * GRAPHIC_WIDTH + xGfxOffset;
				blockSpriteMetaData.frame->y = yOff * GRAPHIC_HEIGHT + yGfxOffset;
				GsSortFastSprite(blockSpriteMetaData.frame, ot, BLOCK_DEPTH);
			}
		}
	}
};


//main//
int	main(void){
	//initialization
	DataManager_Init();
	StartGraphics();
	InitializeText();
	gameScreenState = SCREEN_STATE_START;
	gameOver = false;
	//loading graphics
	blockSpriteSheet = LoadSpriteSheetVram(BLOCK_SPRITE_FILE);
	MakeFullSprite(blocksGraphic, &blockSpriteMetaData, blockSpriteSheet, 0, BLOCK_MAX_COUNT, 0, GRAPHIC_WIDTH, GRAPHIC_HEIGHT, 0, SPR_STATE_DONE, 250);
	LoadBgVram(START_FILE_BG);
	//main loop
	while(true){
		//new frame upkeep, also updates game pads
		UPDATE_OT 
		//screen states
		if (gameScreenState == SCREEN_STATE_GAME){
			//pausing
			if (IsPadTrig(Pad1Start) || IsPadTrig(Pad2Start)){
				LoadBgVram(PAUSE_FILE_BG);
				gameScreenState = SCREEN_STATE_PAUSED;
			}else if (!gameOver){
				//left / right
				if (IsPadTrig(Pad1Left) || IsPadTrig(Pad2Left) || ((IsPadPress(Pad1Left) || IsPadPress(Pad2Left)) && TimerPassed(keyRepeatTimer))){
					keyRepeatTimer = TimerSet(HOLD_DOWN_REPEAT_TIME);
					SetFallingBlockXYrelative(-1, 0);
				}
				if (IsPadTrig(Pad1Right) || IsPadTrig(Pad2Right) || ((IsPadPress(Pad1Right) || IsPadPress(Pad2Right)) && TimerPassed(keyRepeatTimer))){
					keyRepeatTimer = TimerSet(HOLD_DOWN_REPEAT_TIME);
					SetFallingBlockXYrelative(1, 0);
				}
				//rotate
				if (IsPadTrig(Pad1Cross)  || IsPadTrig(Pad2Cross))  SetFallingBlockRotate(-1);
				if (IsPadTrig(Pad1Circle) || IsPadTrig(Pad2Circle)) SetFallingBlockRotate(01);
				//holding
				if (IsPadTrig(Pad1Square) || IsPadTrig(Pad2Square)) HoldBlock();
				//falling
				if (IsPadTrig(Pad1Up) || IsPadTrig(Pad2Up)) quickFall = true;
				//down
				do{ if (quickFall || IsPadTrig(Pad1Down) || IsPadTrig(Pad2Down) || TimerPassed(fallBlockTimer) || ((IsPadPress(Pad1Down) || IsPadPress(Pad2Down)) && TimerPassed(keyRepeatTimer))){
						keyRepeatTimer = TimerSet(HOLD_DOWN_REPEAT_TIME);
						fallBlockTimer = TimerSet(fallBlockTimerValue);
						if (CopyToBoardCheckFallFallingBlock()){
							//this code is for when your done dropping your current block
							quickFall = false;
							heldThisBlock = false;
							if (!GenerateNewFallingBlock()){
								//this is the code if you loose
								gameOver = true; //pause the blocks when game over
							}
						}
					}
				}while(quickFall); //keep going down till you land for quick fall
			}
			//score/level text
			DrawFullBoard();
			DrawBlock(currentFallingBlock, GRAPHIC_X_OFFSET, GRAPHIC_Y_OFFSET, blockFallingX, blockFallingY);
			if (nextFallingId != BLOCK_N) DrawBlock(GetBlockIndex(nextFallingId, 0), BLOCK_NEXT_GRAPHIC_X, BLOCK_NEXT_GRAPHIC_Y, 0, 0);
			if (fallingHoldingId != BLOCK_N) DrawBlock(GetBlockIndex(fallingHoldingId, 0), BLOCK_HELD_GRAPHIC_X, BLOCK_HELD_GRAPHIC_Y, 0, 0);
			sprintf(&DrawTextBuffer, "%04u", lineScore);
			DrawText(SCORE_GRAPHIC_X, SCORE_GRAPHIC_Y);
			sprintf(&DrawTextBuffer, "%02u", SCORE_SPEED_START - fallBlockTimerValue + 1);
			DrawText(LEVEL_GRAPHIC_X, LEVEL_GRAPHIC_Y);
		}else if (gameScreenState == SCREEN_STATE_START){ //start menu
				if (IsPadTrig(Pad1Start) || IsPadTrig(Pad2Start)){//start game button
					LoadBgVram(GAME_FILE_BG);
					gameScreenState = SCREEN_STATE_GAME;
					NewGame();
				}
		}else if (gameScreenState == SCREEN_STATE_PAUSED){ //pause menu
			if (IsPadTrig(Pad1Triangle) || IsPadTrig(Pad2Triangle)){ //new game button
				gameScreenState = SCREEN_STATE_START;
				LoadBgVram(START_FILE_BG);
			} else if (IsPadTrig(Pad1Start) || IsPadTrig(Pad2Start)){ //back to game button
				LoadBgVram(GAME_FILE_BG);
				gameScreenState = SCREEN_STATE_GAME;
				fallBlockTimer = TimerSet(fallBlockTimerValue); //give your block fall time back after unpause
			} else if (IsPadTrig(Pad1Cross) || IsPadTrig(Pad2Cross)){
				LoadBgVram(CREDIT_FILE_BG);
				gameScreenState = SCREEN_STATE_CREDITS;
			}
		}else if (gameScreenState == SCREEN_STATE_CREDITS){ //credits
			if (IsPadTrig(Pad1Cross) || IsPadTrig(Pad2Cross)){
				LoadBgVram(PAUSE_FILE_BG);
				gameScreenState = SCREEN_STATE_PAUSED;
			}
		}
		//drawing
		DrawAllSprites();
	}
}