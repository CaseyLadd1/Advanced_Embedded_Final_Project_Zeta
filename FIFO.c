// FIFO.c
// Runs on any Cortex microcontroller
// Provide functions that initialize a FIFO, put data in, get data out,
// and return the current size.

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Programs 3.7, 3.8., 3.9 and 3.10 in Section 3.7

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "FIFO.h"
#include "os.h"
#include <stdint.h>

// Two-pointer implementation of the receive FIFO
// can hold 0 to RXFIFOSIZE-1 elements

jsDataType volatile *JsPutPt; // put next
jsDataType volatile *JsGetPt; // get next
jsDataType static JsFifo[JSFIFOSIZE];
Sema4Type JsFifoAvailable;

spriteMessage volatile *DrawPutPt; // put next
spriteMessage volatile *DrawGetPt; // get next
spriteMessage static DrawFifo[DRAWFIFOSIZE];
Sema4Type DrawFifoAvailable;
Sema4Type DrawFifoRoom;

soundEvent volatile *SoundPutPt; // put next
soundEvent volatile *SoundGetPt; // get next
soundEvent static SoundFIFO[SOUNDFIFOSIZE];
Sema4Type SoundFIFOAvailable;
Sema4Type SoundFIFORoom;

// initialize pointer FIFO
void JsFifo_Init(void) {
  long sr;
  sr = StartCritical(); // make atomic
  OS_InitSemaphore(&JsFifoAvailable, 0);
  JsPutPt = JsGetPt = &JsFifo[0]; // Empty
  EndCritical(sr);
}
// add element to end of pointer FIFO
// return RXFIFOSUCCESS if successful
int JsFifo_Put(jsDataType data) {
  jsDataType volatile *nextPutPt;
  nextPutPt = JsPutPt + 1;
  if (nextPutPt == &JsFifo[JSFIFOSIZE]) {
    nextPutPt = &JsFifo[0]; // wrap
  }
  if (nextPutPt == JsGetPt) {
    return (JSFIFOFAIL); // Failed, fifo full
  } else {
    *(JsPutPt) = data;   // Put
    JsPutPt = nextPutPt; // Success, update
    OS_Signal(&JsFifoAvailable);
    return (JSFIFOSUCCESS);
  }
}
// remove element from front of pointer FIFO
// return RXFIFOSUCCESS if successful
int JsFifo_Get(jsDataType *datapt) {
  OS_Wait(&JsFifoAvailable);
  *datapt = *(JsGetPt++);
  if (JsGetPt == &JsFifo[JSFIFOSIZE]) {
    JsGetPt = &JsFifo[0]; // wrap
  }
  return (JSFIFOSUCCESS);
}
// number of elements in pointer FIFO
// 0 to RXFIFOSIZE-1
uint32_t JsFifo_Size(void) {
  if (JsPutPt < JsGetPt) {
    return ((uint32_t)(JsPutPt - JsGetPt + (JSFIFOSIZE * sizeof(jsDataType))) /
            sizeof(jsDataType));
  }
  return ((uint32_t)(JsPutPt - JsGetPt) / sizeof(jsDataType));
}

// initialize pointer FIFO
void DrawFifo_Init(void) {
  long sr;
  sr = StartCritical(); // make atomic
  OS_InitSemaphore(&DrawFifoAvailable, 0);
	OS_InitSemaphore(&DrawFifoRoom, DRAWFIFOSIZE);
  DrawPutPt = DrawGetPt = &DrawFifo[0]; // Empty
  EndCritical(sr);
}
// add element to end of pointer FIFO
// return RXFIFOSUCCESS if successful
int DrawFifo_TryPut(spriteMessage data) {
	spriteMessage volatile *nextPutPt;
	if (!OS_Try(&DrawFifoRoom)) return DRAWFIFOFAIL;
	long sr = StartCritical();
	nextPutPt = DrawPutPt + 1;
  if (nextPutPt == &DrawFifo[DRAWFIFOSIZE]) {
    nextPutPt = &DrawFifo[0]; // wrap
  }
	*(DrawPutPt) = data;   // Put
  DrawPutPt = nextPutPt; // Success, update
  OS_Signal(&DrawFifoAvailable);
	EndCritical(sr);
	return DRAWFIFOSUCCESS;
}
void DrawFifo_Put(spriteMessage data) {
	spriteMessage volatile *nextPutPt;
	OS_Wait(&DrawFifoRoom);
	long sr = StartCritical();
	nextPutPt = DrawPutPt + 1;
  if (nextPutPt == &DrawFifo[DRAWFIFOSIZE]) {
    nextPutPt = &DrawFifo[0]; // wrap
  }
	*(DrawPutPt) = data;   // Put
  DrawPutPt = nextPutPt; // Success, update
  OS_Signal(&DrawFifoAvailable);
	EndCritical(sr);
}

// remove element from front of pointer FIFO
// return RXFIFOSUCCESS if successful
int DrawFifo_Get(spriteMessage *datapt) {
  OS_Wait(&DrawFifoAvailable);
	long sr = StartCritical();
  *datapt = *(DrawGetPt++);
  if (DrawGetPt == &DrawFifo[DRAWFIFOSIZE]) {
    DrawGetPt = &DrawFifo[0]; // wrap
  }
	OS_Signal(&DrawFifoRoom);
	EndCritical(sr);
  return (DRAWFIFOSUCCESS);
}
// number of elements in pointer FIFO
// 0 to RXFIFOSIZE-1
uint32_t DrawFifo_Size(void) {
  if (DrawPutPt < DrawGetPt) {
    return ((uint32_t)(DrawPutPt - DrawGetPt + (DRAWFIFOSIZE * sizeof(spriteMessage))) /
            sizeof(spriteMessage));
  }
  return ((uint32_t)(DrawPutPt - DrawGetPt) / sizeof(spriteMessage));
}


// initialize pointer FIFO
void SoundFIFO_Init(void) {
  long sr;
  sr = StartCritical(); // make atomic
  OS_InitSemaphore(&SoundFIFOAvailable, 0);
	OS_InitSemaphore(&SoundFIFORoom, SOUNDFIFOSIZE);
  SoundPutPt = SoundGetPt = &SoundFIFO[0]; // Empty
  EndCritical(sr);
}
// add element to end of pointer FIFO
// return RXFIFOSUCCESS if successful
int SoundFIFO_TryPut(soundEvent data) {
	soundEvent volatile *nextPutPt;
	if (!OS_Try(&SoundFIFORoom)) return SOUNDFIFOFAIL;
	long sr = StartCritical();
	nextPutPt = SoundPutPt + 1;
  if (nextPutPt == &SoundFIFO[SOUNDFIFOSIZE]) {
    nextPutPt = &SoundFIFO[0]; // wrap
  }
	*(SoundPutPt) = data;   // Put
  SoundPutPt = nextPutPt; // Success, update
  OS_Signal(&SoundFIFOAvailable);
	EndCritical(sr);
	return SOUNDFIFOSUCCESS;
}
void SoundFIFO_Put(soundEvent data) {
	soundEvent volatile *nextPutPt;
	OS_Wait(&SoundFIFORoom);
	long sr = StartCritical();
	nextPutPt = SoundPutPt + 1;
  if (nextPutPt == &SoundFIFO[SOUNDFIFOSIZE]) {
    nextPutPt = &SoundFIFO[0]; // wrap
  }
	*(SoundPutPt) = data;   // Put
  SoundPutPt = nextPutPt; // Success, update
  OS_Signal(&SoundFIFOAvailable);
	EndCritical(sr);
}

// remove element from front of pointer FIFO
// return RXFIFOSUCCESS if successful
int SoundFIFO_Get(soundEvent *datapt) {
  OS_Wait(&SoundFIFOAvailable);
	long sr = StartCritical();
  *datapt = *(SoundGetPt++);
  if (SoundGetPt == &SoundFIFO[SOUNDFIFOSIZE]) {
    SoundGetPt = &SoundFIFO[0]; // wrap
  }
	OS_Signal(&SoundFIFORoom);
	EndCritical(sr);
  return (SOUNDFIFOSUCCESS);
}

// remove element from front of pointer FIFO
// return RXFIFOSUCCESS if successful
int SoundFIFO_TryGet(soundEvent *datapt) {
  if (!OS_Try(&SoundFIFOAvailable)) {
		return SOUNDFIFOFAIL;
	}
	long sr = StartCritical();
  *datapt = *(SoundGetPt++);
  if (SoundGetPt == &SoundFIFO[SOUNDFIFOSIZE]) {
    SoundGetPt = &SoundFIFO[0]; // wrap
  }
	OS_Signal(&SoundFIFORoom);
	EndCritical(sr);
  return (SOUNDFIFOSUCCESS);
}

// number of elements in pointer FIFO
// 0 to RXFIFOSIZE-1
uint32_t SoundFIFO_Size(void) {
  if (SoundPutPt < SoundGetPt) {
    return ((uint32_t)(SoundPutPt - SoundGetPt + (SOUNDFIFOSIZE * sizeof(soundEvent))) /
            sizeof(soundEvent));
  }
  return ((uint32_t)(SoundPutPt - SoundGetPt) / sizeof(soundEvent));
}
