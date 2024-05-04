#ifndef EMB_GAMEPLAY_H
#define EMB_GAMEPLAY_H

#include <stdint.h>
#include "os.h"


#define VERTICALNUM 8
#define HORIZONTALNUM 8

typedef struct {
uint8_t position[2];
//uint32_t sprite_offset;
Sema4Type BlockFree;
Sema4Type Touched;
} block;

#ifndef EMB_GAMEPLAY_C_INTERNAL
extern block BlockArray[HORIZONTALNUM][VERTICALNUM];
#else
block BlockArray[HORIZONTALNUM][VERTICALNUM];
#endif


void InitGameplay(void);
void DemonThread(void);
void ShotHandler(void);

#endif