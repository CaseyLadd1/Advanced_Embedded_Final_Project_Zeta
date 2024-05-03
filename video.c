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
	BSP_LCD_DrawBitmap(blockx*16, (blocky+1)*16, BitMapValues+offset, 16, 16, direction);
}

static inline void _clearSprite_internal(uint8_t blockx, uint8_t blocky) {
	BSP_LCD_FillRect(blockx*16, blocky*16, 16, 16, LCD_BLACK);
}

void DrawSprite(uint8_t blockx, uint8_t blocky, uint8_t direction, uint32_t sprite) {
	if (blockx > 7 || blocky > 7 || sprite >= BMP_LENGTH) {
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
	if (blockx > 7 || blocky > 7) {
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
	for (int by = 0; by < 3; by++) {
		for (int bx = 0; bx < 8; bx++) {
			DrawSprite(bx, by, 0, 0x100*(8*by+bx));
		}
	}
	for (int by = 3; by < 6; by++) {
		for (int bx = 0; bx < 7; bx++) {
			DrawSprite(bx, by, 0, 0x100*(3*8+7*(by-3)+bx));
		}
	}
}