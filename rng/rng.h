#include <stdint.h>

#ifndef __RNG_H__
#define __RNG_H__

#define CUBE_DIR_STATIONARY	0x01
#define	CUBE_DIR_DOWN		0x02
#define CUBE_DIR_UP		0x04
#define CUBE_DIR_RIGHT		0x08
#define CUBE_DIR_LEFT		0x10

uint32_t rng(void);

uint32_t GetRandomDirection(void);

#endif
