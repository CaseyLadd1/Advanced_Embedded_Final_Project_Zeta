// gameplay.c
// used to define more specific features of gameplay
#define EMB_GAMEPLAY_C_INTERNAL
#include "gameplay.h"
#include "rng/rng.h"
#include "tm4c123gh6pm.h"
// cursor values need to be passed into this section

const uint32_t ShootSequence[] = {
    BMP_2_1_CAC_ATTTAC_A_OFFSET,  BMP_1_1_CAC_ATTAC_B_OFFSET,
    BMP_1_5_FIREBALLONE_OFFSET,   BMP_1_6_FIREBALLTWO_OFFSET,
    BMP_1_7_FIREBALLTHREE_OFFSET, BMP_2_5_FIREBALLFOUR_OFFSET,
    BMP_2_6_FIREBALLFIVE_OFFSET,  BMP_2_7_FIREBALLSIX_OFFSET
};
const uint32_t ShootSequenceLen = sizeof(ShootSequence) / sizeof(uint32_t);
		
const uint32_t DeathSequence[] = {
	BMP_2_3_CACO_POP_OFFSET, BMP_2_4_WHIRL_OFFSET, BMP_1_4_PARTICLES_OFFSET
};
const uint32_t DeathSequenceLen = sizeof(DeathSequence) / sizeof(uint32_t);

// todo: make semaphore.
Sema4Type ammocount;
Sema4Type lifecount;
Sema4Type levelcount;
Sema4Type score;
Sema4Type livingDemons;
Sema4Type levelRunning;

static void InitBlockArray(void) {
  for (uint8_t bx = 0; bx < HORIZONTALNUM; bx++) {
    for (uint8_t by = 0; by < VERTICALNUM; by++) {
      OS_InitSemaphore(&BlockArray[bx][by].BlockFree, 1);
      OS_InitSemaphore(&BlockArray[bx][by].Touched, 0);
      OS_InitSemaphore(&BlockArray[bx][by].BlockUnoccupied, 1);
      //			BlockArray[bx][by].position[0] = bx*16;
      //			BlockArray[bx][by].position[0] = by*16;
    }
  }
}

void KillAllDemons(void) {
	// So that we don't get the next-level handler.
	OS_InitSemaphore(&livingDemons, 0);
	for (uint8_t bx = 0; bx < HORIZONTALNUM; bx++) {
    for (uint8_t by = 0; by < VERTICALNUM; by++) {
			OS_bSignal(&BlockArray[bx][by].Touched);
			if (!BlockArray[bx][by].BlockUnoccupied.Value) {
				OS_WakeupThread(BlockArray[bx][by].threadId);
			}
		}
	}
}

void InitGameplay(void) {
  InitBlockArray();
  OS_InitSemaphore(&ammocount, MAX_AMMO);
	OS_InitSemaphore(&lifecount, MAX_LIFE);
	OS_InitSemaphore(&levelcount, 0);
	OS_InitSemaphore(&score, 0);
	OS_InitSemaphore(&livingDemons, 0);
	OS_InitSemaphore(&levelRunning, 0);
}

static void AwaitS1_inner(void) {
	OS_bTry(&levelRunning);
	OS_bWait(&levelRunning);
	OS_bSignal(&levelRunning);
}

static void AwaitS1(void) {
	AwaitS1_inner();
	if (NumSamples == RUNLENGTH) OS_Kill();
}

void Consumer(void);
void TitleScreenRoutine(void){
	ClearScreen();
	DrawTitle();
	AwaitS1();
	ClearScreen();
	DrawInstructions();
	AwaitS1();
	ClearScreen();
	UpdateAmmoLife();
	OS_AddThread(&LevelStart, 128, 3);
	OS_AddThread(&Consumer, 128, 1);
	OS_Kill();
}

void ShotHandler(void) {
	OS_bSignal(&levelRunning);
  // called if shot button is pressed
  // calls RNG to "waste" value, further randomizing user experience (1 RNG
  // value total)
  /*if (OS_Try(&ammocount) <= 0) {
    // if no ammo, announce "RELOAD" and return 0
  }*/
	OS_Kill();
}

void ReloadHandler(void) {
  // called if reload button is pressed
	long prevAmmo = ammocount.Value;
	long running = levelRunning.Value;
	// Don't reload if we're paused or if the ammo is already max.
	if (prevAmmo == MAX_AMMO || !running) {
		OS_Kill();
	}
	OS_InitSemaphore(&ammocount, -1);
	UpdateAmmoLife();
	// Calculate how much to delay.
	uint16_t delay = 0;
	// Make delay increase as you get lower on ammo.
	for (int i = MAX_AMMO; i > prevAmmo; i--) {
		delay += rng() & 127;
	}
	for (int i = MAX_AMMO-2; i > prevAmmo; i--) {
		delay += rng() & 127;
	}
	for (int i = MAX_AMMO-4; i > prevAmmo; i--) {
		delay += rng() & 127;
	}
	OS_Sleep(delay*2);
	OS_InitSemaphore(&ammocount, MAX_AMMO);
	UpdateAmmoLife();
	OS_Kill();
}

// For a newly-spawned block. Returns the two block indices (0-7) as a single
// int.
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

static void RunDeathSequence(uint8_t x, uint8_t y) {
	OS_bSignal(&BlockArray[x][y].BlockUnoccupied);
	for (int i = 0; i < DeathSequenceLen; i++) {
		DrawSprite(x, y, 0, DeathSequence[i]);
		OS_Sleep(100);
	}
	ClearSprite(x, y);
  OS_bSignal(&BlockArray[x][y].BlockFree);
	if (OS_Try(&livingDemons) == 1) {
		OS_AddThread(&LevelStart, 128, 3);
	}
	OS_Kill();
}

void Restart(void);
void EndGame(void) {
	NumSamples = RUNLENGTH;
	KillAllDemons();
	OS_Sleep(500);
	ClearScreen();
	UpdateAmmoLife();
	DrawEndScreen();
	AwaitS1_inner();
	OS_AddThread(&Restart, 128, 4);
	OS_Kill();
}

static void RunShootSequence(uint8_t x, uint8_t y) {
	for (int i = 0; i < ShootSequenceLen; i++) {
		DrawSprite(x, y, 0, ShootSequence[i]);
		OS_Sleep(250+200/(i+1) + 200 * (i == ShootSequenceLen - 1));
		if (OS_bTry(&BlockArray[x][y].Touched) && OS_Try(&ammocount) > 0) {
			OS_Signal(&score);
			UpdateAmmoLife();
      // If we have, start the death sequence.
      RunDeathSequence(x, y);
    }
	}
	if (OS_Try(&lifecount) == 1) {
		EndGame();
	}
	UpdateAmmoLife();
	RunDeathSequence(x, y);
}


void LevelStart(void) {
	OS_Signal(&levelcount);
	// Display next-level banner.
	DrawLevelBanner();
	// Await S1 press.
	AwaitS1();
	// Clear the level banner
	ClearPlayArea();
	uint8_t numSpawn = 2 + (rng() & 3);
	if (levelcount.Value > 10) {
		// Make things a bit harder;
		numSpawn += (rng() & 3);
	}
	if (levelcount.Value == 20) {
		EndGame();
	}
	for (int i = 0; i < numSpawn; i++) {
		OS_Signal(&livingDemons);
	  // You filthy demon spawn! Sorry, it just felt surprisingly appropriate.
	  OS_AddThread(&DemonThread, 128, 3);
	}
	OS_Kill();
}

void DemonThread(void) {
  // Sleep between 0 and 255 ms before appearing
  OS_Sleep(rng() & 255u);
  uint8_t pos = FindOpenBlock();
  // For now, just run a very simplified version.
  uint8_t x, y, newx, newy;
	uint8_t lifetime = 6 + (rng() & 7);
  x = pos & 7u;
  y = (pos >> 3) & 7u;
  BlockArray[x][y].threadId = OS_Id();
  OS_bTry(&BlockArray[x][y].Touched);
  OS_bTry(&BlockArray[x][y].BlockUnoccupied);
  DrawSprite(x, y, 0, BMP_2_2_CACO_FRONT_OFFSET);

  for (int i = 0; i < lifetime; i++) {
    OS_Sleep(500 + (rng() & 255));

    // Check if we have been touched.
    if (OS_bTry(&BlockArray[x][y].Touched) && OS_Try(&ammocount) > 0) {
			OS_Signal(&score);
			UpdateAmmoLife();
      // If we have, start the death sequence.
      RunDeathSequence(x, y);
    }

    // Find the next block.
    pos = MoveToOpenBlock(x, y);
    newx = pos & 7u;
    newy = (pos >> 3) & 7u;

    // If the coordinates changed, release the block and redraw.
    if (newx != x || newy != y) {
      // Send a clear-sprite command.
      ClearSprite(x, y);
      // Mark the current block as unoccupied.
      OS_bSignal(&BlockArray[x][y].BlockUnoccupied);

      // write code here for determining which sprite is drawn and in which
      // direction// compare newx and newy to x and y before updating//
      uint8_t facing;
      uint16_t imgID;
      if (newx - x > 0) {
        facing = 1;
        imgID = BMP_1_3_CACO_SIDE_OFFSET;
      }
      if (newx - x < 0) {
        facing = 0;
        imgID = BMP_1_3_CACO_SIDE_OFFSET;
      }
      if (newy - y > 0) {
        facing = 0;
        imgID = BMP_2_2_CACO_FRONT_OFFSET;
      }
      if (newy - y < 0) {
        facing = 0;
        imgID = BMP_1_2_CAC_FACE_BACC_OFFSET;
      }
      // Setup the new block.
      BlockArray[newx][newy].threadId = OS_Id();
      OS_bTry(&BlockArray[newx][newy].Touched);
      OS_bTry(&BlockArray[newx][newy].BlockUnoccupied);
      // Draw the new sprite.
      DrawSprite(newx, newy, facing, imgID);
      // Free the old block.
      OS_bSignal(&BlockArray[x][y].BlockFree);
      x = newx;
      y = newy;
    }
  }
	RunShootSequence(x, y);
}
