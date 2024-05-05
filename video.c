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

static inline void _drawLevelBanner_internal(uint8_t levelnum) {
	BSP_LCD_MessageVar(0, 2, 7, "Level ", levelnum);
	BSP_LCD_DrawString(0, 10, "Press S1 to continue.", LCD_WHITE);
}

static inline void _clearLevelBanner_internal() {
	BSP_LCD_FillRect(0, 0, 127, 111, LCD_BLACK);
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
void ClearLevelBanner(void) {
	DrawFifo_Put((spriteMessage){
		.command=4
	});
}

extern unsigned long NumSamples;
#define RUNLENGTH 600
extern Sema4Type LCDFree;
void RenderThread(void) {
	while (NumSamples < RUNLENGTH) {
    spriteMessage data;
    DrawFifo_Get(&data);
    OS_bWait(&LCDFree);
		if (data.command == 1) {
			_clearSprite_internal(data.blockx, data.blocky);
		} else if (data.command == 2) {
			_updateAmmoLife_internal(ammocount.Value, lifecount.Value, score.Value);
		} else if (data.command == 3) {
			_drawLevelBanner_internal(levelcount.Value);
		} else if (data.command == 4) {
			_clearLevelBanner_internal();
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