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

static inline void _drawSprite_internal(uint8_t blockx, uint8_t blocky, uint8_t direction, uint32_t offset) {
	BSP_LCD_DrawBitmap(blockx*16, (blocky+1)*16-1, &BitMapValues[offset], 16, 16, direction);
}


static inline void _clearSprite_internal(uint8_t blockx, uint8_t blocky) {
	BSP_LCD_FillRect(blockx*16, blocky*16, 16, 16, LCD_BLACK);
}

void DrawSprite(uint8_t blockx, uint8_t blocky, uint8_t direction, uint32_t sprite) {
	if (blockx > HORIZONTALNUM || blocky > VERTICALNUM || sprite >= BMP_LENGTH) {
		return;
	}
	// TODO: make blocking.
	DrawFifo_Put((spriteMessage){
		.blockx=blockx, .blocky=blocky,
		.direction=direction,
		.clear=0,
		.sprite=sprite
	});
}
void ClearSprite(uint8_t blockx, uint8_t blocky) {
	if (blockx > HORIZONTALNUM || blocky > VERTICALNUM) {
		return;
	}
	DrawFifo_Put((spriteMessage){
		.blockx=blockx, .blocky=blocky,
		.clear=1
	});
}

extern unsigned long NumSamples;
#define RUNLENGTH 600
extern Sema4Type LCDFree;
void SpriteRenderThread(void) {
	while (NumSamples < RUNLENGTH) {
    spriteMessage data;
    DrawFifo_Get(&data);
    OS_bWait(&LCDFree);
		if (data.clear) {
			_clearSprite_internal(data.blockx, data.blocky);
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

void offsetTest(void) {
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

void DrawTitle(void){
	DrawSprite(1,2,0, 0x1000);
	DrawSprite(2,2,0, 0x1100);
	DrawSprite(3,2,0, 0x1200);
	DrawSprite(4,2,0, 0x1300);
	DrawSprite(5,2,0, 0x1400);
	DrawSprite(6,2,0, 0x1500);
	DrawSprite(7,2,0, 0x1600);
	DrawSprite(1,3,0, 0x1800);
	DrawSprite(2,3,0, 0x1900);
	DrawSprite(3,3,0, 0x1a00);
	DrawSprite(4,3,0, 0x1b00);
	DrawSprite(5,3,0, 0x1c00);
	DrawSprite(6,3,0, 0x1d00);
	DrawSprite(7,3,0, 0x1e00);
	DrawSprite(1,4,0, 0x1f00);
	DrawSprite(2,4,0, 0x2000);
	DrawSprite(3,4,0, 0x2100);
	DrawSprite(4,4,0, 0x2200);
	DrawSprite(5,4,0, 0x2300);
	DrawSprite(6,4,0, 0x2400);
	DrawSprite(7,4,0, 0x2500);
	DrawSprite(1,5,0, 0x2600);
	DrawSprite(2,5,0, 0x2700);
	DrawSprite(3,5,0, 0x2800);
	DrawSprite(4,5,0, 0x2900);
	DrawSprite(5,5,0, 0x2a00);
	DrawSprite(6,5,0, 0x2b00);
	DrawSprite(7,5,0, 0x2c00);

}