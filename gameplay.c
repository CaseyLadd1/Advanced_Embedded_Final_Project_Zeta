// gameplay.c
//used to define more specific features of gameplay
#define EMB_GAMEPLAY_C_INTERNAL
#include "gameplay.h"
#include "tm4c123gh6pm.h"
#include "rng/rng.h"
//cursor values need to be passed into this section

// todo: make semaphore.
static uint16_t ammocount;
static uint16_t lifecount;


static void InitBlockArray(void) {
	for (uint8_t bx = 0; bx < HORIZONTALNUM; bx++) {
		for (uint8_t by = 0; by < VERTICALNUM; by++) {
			OS_InitSemaphore(&BlockArray[bx][by].BlockFree, 1);
			OS_InitSemaphore(&BlockArray[bx][by].Touched, 0);
			BlockArray[bx][by].position[0] = bx*16;
			BlockArray[bx][by].position[0] = by*16;
		}
	}
}

void InitGameplay(void){
	InitBlockArray();
	ammocount = 6;
	lifecount = 5;
	//code goes here
	//establishes initial values of life, ammo
}

void ShotHandler(void){
	//called if shot button is pressed
	//calls RNG to "waste" value, further randomizing user experience (1 RNG value total)
	if(ammocount<=0){
	//if no ammo, announce "RELOAD" and return 0
	}
	if(ammocount>0){
		ammocount--; //if ammo, ammo--, check if cursor pos = pos of cocoa thread
		/*center of cursor should be within a certain x range and a certain y range
		  these ranges are attached to a cocademon thread as "xmin" "xmax" "ymin" and "ymax"
		  if cursor pos = cocoa pos, score++,
		  pass cocoa a value to indicate it should run OS_Kill()
		*/
	  //return 1
	}
}
// Made these static for now.
static void reload_handler(void){
	//called if reload button is pressed
	//get current ammo value
	//calls RNG to "waste" value, further randomizing user experience (1 RNG value total if current ammo is odd, 2 if even, 3 if zero)
	//set ammo = 6 and freeze user inputs for a time
	//return 1
}

static void cocoademon_handler(void){
	//code goes here
	//calls RNG to determine if a new cocoademon thread is created (1 RNG value total), returns 0 if false
	//checks RNG for cocoademon position, color, lifetime (4 RNG values total)
}

// For a newly-spawned block. Returns the two block indices (0-7) as a single int.
static uint8_t FindOpenBlock(void) {
	uint32_t rand;
	uint8_t x, y;
	int open = 0;
	while (!open) {
		rand = rng();
		x = rand & 7u;
		y = (rand >> 3) & 7u;
		if (x >= HORIZONTALNUM || y >= VERTICALNUM) {
			continue;
		}
		open = OS_bTry(&BlockArray[x][y].BlockFree);
	}
	return (y << 3) | x;
}

static uint8_t MoveToOpenBlock(uint8_t x, uint8_t y) {
	int8_t newx = x, newy = y, dir;
	int open = 0;
	while (!open) {
		dir = GetRandomDirection();
		if (dir == 4) {
			return (y << 3) | x;
		}
		// bit 0 of dir: interpret as x or y movement.
		// bit 1 of dir: interpret as - or +.
		newx = x + (dir & 1) * (1 - (dir & 2));
		newy = y + (!(dir & 1)) * (1 - (dir & 2));
		if (newx >= HORIZONTALNUM || newy >= VERTICALNUM || newx < 0 || newy < 0) {
			continue;
		}
		open = OS_bTry(&BlockArray[newx][newy].BlockFree);
	}
	return (newy << 3) | newx;
}

void DemonThread(void){
	// Sleep between 0 and 255 ms before appearing
	OS_Sleep(rng() & 255u);
	uint8_t pos = FindOpenBlock();
	// For now, just run a very simplified version.
	uint8_t x, y, newx, newy;
	x = pos & 7u;
	y = (pos >> 3) & 7u;
	DrawSprite(x, y, 0, 0);
for (int i = 0; i < 1000; i++) {
OS_bTry(&BlockArray[x][y].Touched);
OS_Sleep(500);
if (OS_bTry(&BlockArray[x][y].Touched)) {
	OS_bSignal(&BlockArray[x][y].BlockFree);
	ClearSprite(x, y);
	break;
}
pos = MoveToOpenBlock(x, y);
	newx = pos & 7u;
	newy = (pos >> 3) & 7u;
if (newx != x || newy != y) {
	ClearSprite(x, y);
	OS_bSignal(&BlockArray[x][y].BlockFree);
	x = newx;
y = newy;
	DrawSprite(x, y, 0, 0);
}
}
OS_Kill();
	//runs as a thread for each active instance of cocoademon
	//when lifetime=0 or is defeated (value passed from shot_handler), run OS_Kill()
}

