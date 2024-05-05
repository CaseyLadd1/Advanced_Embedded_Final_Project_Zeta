// video.c
// handles video elements, contains graphical data
// Runs on LM4F120/TM4C123, as part of the MOOD MINI Project
// Sarah Hemler 4/25/24, seh5pp@virginia.edu
// Modified by John Berberian
// Modified by Paul Karhnak
// Modified by Casey Ladd
#define DEFINE_BITMAP
#include "video.h"
#include "LCD.h"
#include "FIFO.h"
#include "os.h"
#include "gameplay.h"

static inline void _drawSprite_internal(uint8_t blockx, uint8_t blocky, uint8_t direction, uint32_t offset) {
	BSP_LCD_DrawBitmap(blockx*16, (blocky+1)*16-1, &BitMapValues[offset], 16, 16, direction);
}


static inline void _clearSprite_internal(uint8_t blockx, uint8_t blocky) {
	BSP_LCD_FillRect(blockx*16, blocky*16, 16, 16, LCD_BLACK);
}

static inline void _updateAmmoLife_internal(long ammo, uint8_t life, uint8_t score) {
	BSP_LCD_MessageVar(1, 5, 0, "HP: ", life);
	BSP_LCD_MessageVar(1, 5, 7, "PT: ", score);
	if (ammo >= 0) {
	  BSP_LCD_MessageVar(1, 5, 15, "A: ", ammo);
		BSP_LCD_MessageVar(1, 5, 19, "/", MAX_AMMO);
	} else {
		BSP_LCD_MessageVar(1, 5, 15, "A: R/", MAX_AMMO);
	}
	
}

static void _promptS1_internal(void) {
	BSP_LCD_DrawString(0, 10, "Press S1 to continue.", LCD_WHITE);
}

static inline void _drawLevelBanner_internal(uint8_t levelnum) {
	BSP_LCD_MessageVar(0, 2, 7, "Level ", levelnum);
	_promptS1_internal();
}

static inline void _clearPlayArea_internal() {
	BSP_LCD_FillRect(0, 0, 127, 111, LCD_BLACK);
}

static void _drawTitle_internal(void) {
	BSP_LCD_DrawBitmap(8*1, 16*2, &BitMapValues[0x1000], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*3, 16*2, &BitMapValues[0x1100], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*5, 16*2, &BitMapValues[0x1200], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*7, 16*2, &BitMapValues[0x1300], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*9, 16*2, &BitMapValues[0x1400], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*11, 16*2, &BitMapValues[0x1500], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*13, 16*2, &BitMapValues[0x1600], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*1, 16*3, &BitMapValues[0x1800], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*3, 16*3, &BitMapValues[0x1900], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*5, 16*3, &BitMapValues[0x1a00], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*7, 16*3, &BitMapValues[0x1b00], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*9, 16*3, &BitMapValues[0x1c00], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*11, 16*3, &BitMapValues[0x1d00], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*13, 16*3, &BitMapValues[0x1e00], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*1, 16*4, &BitMapValues[0x1f00], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*3, 16*4, &BitMapValues[0x2000], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*5, 16*4, &BitMapValues[0x2100], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*7, 16*4, &BitMapValues[0x2200], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*9, 16*4, &BitMapValues[0x2300], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*11, 16*4, &BitMapValues[0x2400], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*13, 16*4, &BitMapValues[0x2500], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*1, 16*5, &BitMapValues[0x2600], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*3, 16*5, &BitMapValues[0x2700], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*5, 16*5, &BitMapValues[0x2800], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*7, 16*5, &BitMapValues[0x2900], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*9, 16*5, &BitMapValues[0x2a00], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*11, 16*5, &BitMapValues[0x2b00], 16, 16, 0);
	BSP_LCD_DrawBitmap(8*13, 16*5, &BitMapValues[0x2c00], 16, 16, 0);
}

static void _drawInstructions_internal(void) {
	BSP_LCD_DrawString(0, 0, "Instructions:", LCD_WHITE);
	BSP_LCD_DrawString(2, 2, "Use Joystick to AIM", LCD_WHITE);
	BSP_LCD_DrawString(2, 3, "Press S1 to FIRE", LCD_WHITE);
	BSP_LCD_DrawString(2, 4, "Press S2 to RELOAD", LCD_WHITE);
	BSP_LCD_DrawString(2, 5, "Press Joystick to", LCD_WHITE);
	BSP_LCD_DrawString(13, 6, "RESTART", LCD_WHITE);
	//blink this
	_promptS1_internal();
}

static void _drawEndScreen_internal(void) {
	BSP_LCD_DrawString(5, 1, "GAME OVER!", LCD_WHITE);
	
	BSP_LCD_DrawString(0, 4, "Made by:", LCD_WHITE);
	BSP_LCD_DrawString(2, 5, "John Berberian", LCD_WHITE);
	BSP_LCD_DrawString(2, 6, "Sarah Hemler", LCD_WHITE);
	BSP_LCD_DrawString(2, 7, "Paul Karhnak", LCD_WHITE);
	BSP_LCD_DrawString(2, 8, "Casey Ladd", LCD_WHITE);
	
	BSP_LCD_DrawString(0, 10, "Press S1 to restart.", LCD_WHITE);
}

void DrawSprite(uint8_t blockx, uint8_t blocky, uint8_t direction, uint32_t sprite) {
	if (blockx > HORIZONTALNUM || blocky > VERTICALNUM || sprite >= BMP_LENGTH) {
		return;
	}
	DrawFifo_Put((spriteMessage){
		.blockx=blockx, .blocky=blocky,
		.direction=direction,
		.command=0,
		.sprite=sprite
	});
}

void ClearSprite(uint8_t blockx, uint8_t blocky) {
	if (blockx > HORIZONTALNUM || blocky > VERTICALNUM) {
		return;
	}
	DrawFifo_Put((spriteMessage){
		.blockx=blockx, .blocky=blocky,
		.command=1
	});
}

void UpdateAmmoLife(void) {
	DrawFifo_Put((spriteMessage){
		.command=2
	});
}

void DrawLevelBanner(void) {
	DrawFifo_Put((spriteMessage){
		.command=3
	});
}
void ClearPlayArea(void) {
	DrawFifo_Put((spriteMessage){
		.command=4
	});
}

void ClearScreen(void) {
	DrawFifo_Put((spriteMessage){
		.command=5
	});
}

void DrawTitle(void) {
	DrawFifo_Put((spriteMessage){
		.command=6
	});
}

void DrawInstructions(void) {
	DrawFifo_Put((spriteMessage){
		.command=7
	});
}

void DrawEndScreen(void) {
	DrawFifo_Put((spriteMessage){
		.command=8
	});
}

extern unsigned long NumSamples;
#define RUNLENGTH 600
extern Sema4Type LCDFree;
void RenderThread(void) {
	spriteMessage data;
	while (1) {
    DrawFifo_Get(&data);
    OS_bWait(&LCDFree);
		if (data.command == 1) {
			_clearSprite_internal(data.blockx, data.blocky);
		} else if (data.command == 2) {
			_updateAmmoLife_internal(ammocount.Value, lifecount.Value, score.Value);
		} else if (data.command == 3) {
			_drawLevelBanner_internal(levelcount.Value);
		} else if (data.command == 4) {
			_clearPlayArea_internal();
		} else if (data.command == 5) {
			BSP_LCD_FillScreen(LCD_BLACK);
		} else if (data.command == 6) {
			_drawTitle_internal();
			_promptS1_internal();
		} else if (data.command == 7) {
			_drawInstructions_internal();
		} else if (data.command == 8) {
			_drawEndScreen_internal();
		} else {
			_drawSprite_internal(data.blockx, data.blocky, data.direction, data.sprite);
		}
    OS_bSignal(&LCDFree);
  }
  OS_Kill(); // done
}

void RenderInit(void) {
	DrawFifo_Init();
}

void ShowSpriteTest(void) {
	uint8_t bx, by;
	for (by = 0; by < 3; by++) {
		for (bx = 0; bx < 8; bx++) {
			DrawSprite(bx, by, 0, 0x100*(8*by+bx));
		}
	}
	for (by = 3; by < 6; by++) {
		for (bx = 0; bx < 7; bx++) {
			DrawSprite(bx, by, 0, 0x100*(3*8+7*(by-3)+bx));
		}
	}
}