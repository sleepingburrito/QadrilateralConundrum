#include "Sprite.h"

//startup items
#define SCREEN_WIDTH 256
#define SCREEN_WIDTH_HALF (SCREEN_WIDTH / 2)
#define SCREEN_HIGHT 240
#define SCREEN_HIGHT_HALF (SCREEN_HIGHT / 2)

#define MAX_SPRITES 255

#define FONT_MAX 128
#define FONT_WIDTH 8
#define FONT_HEIGHT FONT_WIDTH
#define FONT_FILE "FONT.TIM"
#define DRAW_TEXT_MAX 24
#define DRAW_TEXT_SPRITE_DEPTH 0

enum{
	SPR_STATE_PLAY,
	SPR_STATE_DONE,
	SPR_STATE_LOOPING,
};

typedef struct{
	GsSPRITE * startFrame;
	GsSPRITE * endFrame;
	GsSPRITE * frame;
	unsigned char delay, state, depth;
	unsigned int timer;
}FullSprite;

char DrawTextBuffer[DRAW_TEXT_MAX + 1]; //buffer that DrawText uses
GsSPRITE fontSpr[FONT_MAX];

unsigned char SpritesCount = 0;
FullSprite * FullSprites[MAX_SPRITES];

GsOT * ot;

#define BG_DEPTH 254
GsSPRITE bgSpriteBuffer;

//
//sprites / bg
//
#define UPDATE_OT ot = System_InitFrame();

GsIMAGE * LoadSpriteSheetVram(char * fileName){
	return Tim_Load(LoadCDFileBuffer(fileName));
}

void MakeFullSprite(
GsSPRITE * outSprites, //holds sprite data, must be allocated, sprite sheets must be left to right
FullSprite * fullSpriteOut, //hold animation data, must be allocated
GsIMAGE * spriteSheet,
const unsigned char startFrameIndex,
const unsigned char count,
const unsigned char yOffset,
const unsigned char width,
const unsigned char height,
const unsigned char delay,
const unsigned char state,
const unsigned char depth){
	unsigned char i = startFrameIndex;
	
	//make sprites
	do{ Sprite_Init(&outSprites[i], spriteSheet, SPRITE_NORMAL, width * i, yOffset, width, height);
	}while(++i < count);
	
	//make full sprite
	fullSpriteOut->frame = fullSpriteOut->startFrame = outSprites;
	fullSpriteOut->endFrame = &outSprites[count - 1];
	fullSpriteOut->delay = delay;
	fullSpriteOut->state = state;
	fullSpriteOut->depth = depth;
	fullSpriteOut->timer = 0;
}

void LoadBgVram(char * fileName){
	Sprite_Init(&bgSpriteBuffer, Tim_Load(LoadCDFileBuffer(fileName)), SPRITE_NORMAL, 0, 0, SCREEN_WIDTH, SCREEN_HIGHT);
}

void AddSprite(FullSprite * spr, const unsigned char x, const unsigned y){
		spr->frame->x = x;
		spr->frame->y = y;
				
		FullSprites[SpritesCount++] = spr;
}

void DrawAllSprites(void){ //must come last to draw all sprites and text
	//draw bg
	GsSortFastSprite(&bgSpriteBuffer, ot, BG_DEPTH);
	
	//draw sprites
	while(SpritesCount){
		FullSprite * tmpSpr = FullSprites[--SpritesCount];
		//animation logic
		if (tmpSpr->state != SPR_STATE_DONE){
			if (TimerPassed(tmpSpr->timer)){
				if (tmpSpr->frame != tmpSpr->endFrame){
					++(tmpSpr->frame);
				} else if (tmpSpr->state == SPR_STATE_LOOPING) {
					tmpSpr->frame = tmpSpr->startFrame;
				} else {
					tmpSpr->state = SPR_STATE_DONE;
				}
				tmpSpr->timer = TimerSet(tmpSpr->delay);
			}
		}
		//drawing sprites
		GsSortFastSprite(tmpSpr->frame, ot, tmpSpr->depth);
	}
	
	//draw to screen
	System_DrawFrame(ot);
}

void StartGraphics(void){
	System_Init(MODE_NTSC, VMODE_16BITS, SCREEN_WIDTH, SCREEN_HIGHT, GsNONINTER, 0);
}


//
//bitmap font
//
void InitializeText(void){
	//loading bitmap font
	unsigned char i = 0;
	GsIMAGE * spriteSheetBuffer;
	spriteSheetBuffer = Tim_Load(LoadCDFileBuffer(FONT_FILE));
	do{ Sprite_Init(&fontSpr[i], spriteSheetBuffer, SPRITE_NORMAL, i * FONT_WIDTH, 0, FONT_WIDTH, FONT_HEIGHT);
	}while(++i < FONT_MAX);
	
	//max draw
	DrawTextBuffer[DRAW_TEXT_MAX + 1] = 0;
}

void DrawText(unsigned char x, unsigned char y){ //use DrawTextBuffer to set text
	unsigned char xStart = x;
	char * str = DrawTextBuffer;
	GsSPRITE * letter;
	do{ if (*str == '\n'){
			x = xStart;
			y += FONT_HEIGHT;
		} else {
			letter = &fontSpr[*str];
			letter->x = x;
			x += FONT_WIDTH;
			letter->y = y;
			GsSortFastSprite(letter, ot, DRAW_TEXT_SPRITE_DEPTH);
		}
	}while(*(++str));
}