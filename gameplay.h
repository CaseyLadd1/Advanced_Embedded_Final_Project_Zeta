#ifndef EMB_GAMEPLAY_H
#define EMB_GAMEPLAY_H

#include <stdint.h>
#include "os.h"
#include "video.h"

// Free and Unoccupied have subtly different meanings here.
// A block is Free if it is not currently allocated by a thread.
// A block is Unoccupied if there is not currently a sprite drawn in it.
// Any block that is not Unoccupied must have a valid threadId written.
// A valid threadId is not required for blocks that are simply not Free.
// Threads should reset Touched right before they change Unoccupied.
typedef struct {
//uint8_t position[2];
Sema4Type BlockFree;
Sema4Type BlockUnoccupied;
Sema4Type Touched;
unsigned long threadId;
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