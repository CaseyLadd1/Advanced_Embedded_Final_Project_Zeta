#include <stdint.h>
#include "rng/rng.h"

#define LCD_MIN_WIDTH	(int16_t) 0
#define LCD_MAX_WIDTH	(int16_t) 128
#define LCD_MIN_HEIGHT	(int16_t) 0
#define LCD_MAX_HEIGHT	(int16_t) 128
#define CELL_WIDTH	(int16_t) 16
#define CELL_HEIGHT	(int16_t) 16

#define LCD_BLACK	0x0000

uint8_t ValidateCoordinates(uint16_t CurrentX, uint16_t CurrentY, uint16_t NewX, uint16_t NewY) {
    // Acceptable column numbers (to get multiple of 16 such that 0 <= x < 128): 0, 1, 2, 3, 4, 5, 6, 7
    // Acceptable row numbers (to get multiple of 16 such that 0 <= y < 128): 0, 1, 2, 3, 4, 5, 6, 7
    if ( ((NewX * CELL_WIDTH) < LCD_MIN_WIDTH) || ((NewX * CELL_WIDTH) >= LCD_MAX_WIDTH) ) {
	return 0; // Logical "false"
    } else if ( ((NewY * CELL_HEIGHT) < LCD_MIN_HEIGHT) || ((NewY * CELL_HEIGHT) >= LCD_MAX_HEIGHT) ) {
	return 0; // Logical "false"
    }

    return 1; // Else, x and y coordinates are within necessary LCD bounds to correctly draw bitmap. Return logical true
}

void MoveCube(uint16_t CurrentX, uint16_t CurrentY, uint16_t *Bitmap, uint16_t Width, uint16_t Height) {
    uint8_t NextPositionValidated = 0;
    uint16_t NewX, NewY;

    while (NextPositionValidated == 0) {
	uint16_t NextMoveDirection = (uint16_t) GetRandomDirection();

	if (NextMoveDirection & CUBE_DIR_DOWN) {
	    NextPositionValidated = ValidateCoordinates(CurrentX, CurrentY, CurrentX, CurrentY + 1);
	    NewX = CurrentX;
	    NewY = CurrentY + 1;
	} else if (NextMoveDirection & CUBE_DIR_LEFT) {
	    NextPositionValidated = ValidateCoordinates(CurrentX, CurrentY, CurrentX - 1, CurrentY);
	    NewX = CurrentX - 1;
	    NewY = CurrentY;
	} else if (NextMoveDirection & CUBE_DIR_UP) {
	    NextPositionValidated = ValidateCoordinates(CurrentX, CurrentY, CurrentX, CurrentY - 1);
	    NewX = CurrentX;
	    NewY = CurrentY - 1;
	} else if (NextMoveDirection & CUBE_DIR_RIGHT) {
	    NextPositionValidated = ValidateCoordinates(CurrentX, CurrentY, CurrentX + 1, CurrentY);
	    NewX = CurrentX + 1;
	    NewY = CurrentY;
	} else {
	    // a shorthand to simply redraw the cube at the current position, which is assumed valid
	    NextPositionValidated = 1;
	    NewX = CurrentX;
	    NewY = CurrentY;
	}
    }

    BSP_LCD_FillRect(CurrentX, CurrentY, Width, Height, LCD_BLACK);
    BSP_LCD_DrawBitmap(NewX, NewY, Bitmap, Width, Height);
}
