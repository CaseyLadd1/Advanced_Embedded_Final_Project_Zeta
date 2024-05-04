#ifndef EMB_VIDEO_H
#define EMB_VIDEO_H

#include "graphics/MOOD_graphics.h"

#define VERTICALNUM 7
#define HORIZONTALNUM 8


void DrawSprite(uint8_t blockx, uint8_t blocky, uint8_t direction, uint32_t sprite);
void ClearSprite(uint8_t blockx, uint8_t blocky);
void UpdateAmmoLife();
void RenderThread(void);
void RenderInit(void);
void ShowSpriteTest(void);

#endif

