#ifndef EMB_VIDEO_H
#define EMB_VIDEO_H

#include "graphics/MOOD_graphics.h"

#define VERTICALNUM 7
#define HORIZONTALNUM 8


void DrawSprite(uint8_t blockx, uint8_t blocky, uint8_t direction, uint32_t sprite);
void offsetTest(void);
void ClearSprite(uint8_t blockx, uint8_t blocky);
void UpdateAmmoLife(void);
void DrawLevelBanner(void);
void ClearPlayArea(void);
void ClearScreen(void);
void DrawTitle(void);
void DrawInstructions(void);
void RenderThread(void);
void RenderInit(void);
void ShowSpriteTest(void);
void DrawTitle(void);
#endif

