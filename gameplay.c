// gameplay.c
//used to define more specific features of gameplay
#define EMB_GAMEPLAY_C_INTERNAL
#include "gameplay.h"
#include "tm4c123gh6pm.h"
#include "video.h"
//cursor values need to be passed into this section

// todo: make semaphore.
static uint16_t ammocount;
static uint16_t lifecount;


static void InitBlockArray(void) {
	for (int bx = 0; bx < HORIZONTALNUM; bx++) {
		for (int by = 0; by < VERTICALNUM; by++) {
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

void DemonThread(void){
	// For now, just run a very simplified version.
	uint8_t blockx, blocky = 0;
	// Mark block as not free.
	OS_bWait(&BlockArray[blockx][blocky].BlockFree);
	
DrawSprite(blockx, blocky, 0, 0);
OS_bWait(&BlockArray[blockx][blocky].Touched);
ClearSprite(blockx, blocky);
OS_Kill();
	//runs as a thread for each active instance of cocoademon
	//when lifetime=0 or is defeated (value passed from shot_handler), run OS_Kill()
}

