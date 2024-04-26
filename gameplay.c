// gameplay.c
//used to define more specific features of gameplay

#include "tm4c123gh6pm.h"
#include <stdint.h>
//cursor values need to be passed into this section

static uint16_t ammocount;
static uint16_t lifecount;

void gameplayInit(void){
	ammocount = 6;
	lifecount = 5;
	//code goes here
	//establishes initial values of life, ammo
}

void shot_handler(void){
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

void reload_handler(void){
	//called if reload button is pressed
	//get current ammo value
	//calls RNG to "waste" value, further randomizing user experience (1 RNG value total if current ammo is odd, 2 if even, 3 if zero)
	//set ammo = 6 and freeze user inputs for a time
	//return 1
}

void cocoademon_handler(void){
	//code goes here
	//calls RNG to determine if a new cocoademon thread is created (1 RNG value total), returns 0 if false
	//checks RNG for cocoademon position, color, lifetime (4 RNG values total)
}

void cocoademon_instance(){
	//code goes here
	//runs as a thread for each active instance of cocoademon
	//when lifetime=0 or is defeated (value passed from shot_handler), run OS_Kill()
}

