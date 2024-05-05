// Main.c
// Runs on LM4F120/TM4C123
// You may use, edit, run or distribute this file
// You are free to change the syntax/organization of this file

// Jonathan W. Valvano 2/20/17, valvano@mail.utexas.edu
// Modified by Sile Shu 10/4/17, ss5de@virginia.edu
// Modified by Mustafa Hotaki 7/29/18, mkh3cf@virginia.edu

#include "FIFO.h"
#include "LCD.h"
#include "os.h"
#include "PORTE.h"
#include "UART.h"
#include "joystick.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <string.h>
#include "video.h"
#include "gameplay.h"

// Constants
#define BGCOLOR LCD_BLACK
#define CROSSSIZE 5
#define PERIOD 4000000 // DAS 20Hz sampling period in system time units
#define PSEUDOPERIOD 8000000
#define LIFETIME 1000


extern Sema4Type LCDFree;
uint16_t origin[2];   // The original ADC value of x,y if the joystick is not
                      // touched, used as reference
int16_t x = 63;       // horizontal position of the crosshair, initially 63
int16_t y = 63;       // vertical position of the crosshair, initially 63
int16_t prevx, prevy; // Previous x and y values of the crosshair

unsigned long NumSamples;  // Incremented every ADC sample, in Producer
unsigned long UpdateWork;  // Incremented every update on position values
unsigned long Calculation; // Incremented every cube number calculation
unsigned long
    DisplayCount; // Incremented every time the Display thread prints on LCD
unsigned long
    ConsumerCount; // Incremented every time the Consumer thread prints on LCD
unsigned long Button1RespTime; // Latency for Task 2 = Time between button1 push
                               // and response on LCD
unsigned long Button2RespTime; // Latency for Task 7 = Time between button2 push
                               // and response on LCD
unsigned long Button1PushTime; // Time stamp for when button 1 was pushed
unsigned long Button2PushTime; // Time stamp for when button 2 was pushed

//---------------------User debugging-----------------------
unsigned long DataLost; // data sent by Producer, but not received by Consumer
long MaxJitter;         // largest time jitter between interrupts in usec
#define JITTERSIZE 64
unsigned long const JitterSize = JITTERSIZE;
unsigned long JitterHistogram[JITTERSIZE] = {
    0,
};
unsigned long TotalWithI1;
unsigned short MaxWithI1;

void Device_Init(void) {
  UART_Init();
  BSP_LCD_OutputInit();
  BSP_Joystick_Init();
}
//------------------Task 1--------------------------------
// background thread executed at 20 Hz
//******** Producer ***************
int UpdatePosition(uint16_t rawx, uint16_t rawy, jsDataType *data) {
  if (rawx > origin[0]) {
    x = x + ((rawx - origin[0]) >> 9);
  } else {
    x = x - ((origin[0] - rawx) >> 9);
  }
  if (rawy < origin[1]) {
    y = y + ((origin[1] - rawy) >> 9);
  } else {
    y = y - ((rawy - origin[1]) >> 9);
  }
  if (x > 127) {
    x = 127;
  }
  if (x < 0) {
    x = 0;
  }
  if (y > 112-1) {
    y = 112-1;
  }
  if (y < 0) {
    y = 0;
  }
  data->x = x;
  data->y = y;
  return 1;
}

void Restart(void);

void Producer(void) {
  uint16_t rawX, rawY; // raw adc value
  uint8_t select;
  jsDataType data;
  unsigned static long LastTime; // time at previous ADC sample
  unsigned long thisTime;        // time at current ADC sample
  long jitter;                   // time between measured and expected, in us
  if (NumSamples < RUNLENGTH) {
    BSP_Joystick_Input(&rawX, &rawY, &select);
    thisTime = OS_Time();                            // current time, 12.5 ns
    UpdateWork += UpdatePosition(rawX, rawY, &data); // calculation work
    //NumSamples++;                                    // number of samples
    if (JsFifo_Put(data) == 0) {                     // send to consumer
      DataLost++;
    }
		if (select == 0) {
		OS_AddThread(&Restart, 128, 4);
		}
    // calculate jitter
    if (UpdateWork > 1) { // ignore timing of first interrupt
      unsigned long diff = OS_TimeDifference(LastTime, thisTime);
      if (diff > PERIOD) {
        jitter = (diff - PERIOD + 4) / 8; // in 0.1 usec
      } else {
        jitter = (PERIOD - diff + 4) / 8; // in 0.1 usec
      }
      if (jitter > MaxJitter) {
        MaxJitter = jitter; // in usec
      }                     // jitter should be 0
      if (jitter >= JitterSize) {
        jitter = JITTERSIZE - 1;
      }
      JitterHistogram[jitter]++;
    }
    LastTime = thisTime;
  }
}

//--------------end of Task 1-----------------------------


//************SW1Push*************
// Called when SW1 Button pushed
// Adds another foreground task
// background threads execute once and return
void SW1Push(void) {
  Button1PushTime = OS_Time();
  if (OS_MsTime() > 20) { // debounce
    if (OS_AddThread(&ShotHandler, 128, 2)) {
      OS_ClearMsTime();
    }
    OS_ClearMsTime(); // at least 20ms between touches
	}
}

//--------------end of Task 2-----------------------------

//------------------Task 3--------------------------------

//******** Consumer ***************
// foreground thread, accepts data from producer
// Display crosshair and its positions
// inputs:  none
// outputs: none
void Consumer(void) {
	uint8_t bx, by;
  while (NumSamples < RUNLENGTH) {
    jsDataType data;
    JsFifo_Get(&data);
    OS_bWait(&LCDFree);

    BSP_LCD_DrawCrosshair(prevx, prevy, LCD_BLACK); // Draw a black crosshair
    BSP_LCD_DrawCrosshair(data.x, data.y, LCD_RED); // Draw a red crosshair
		
		OS_bSignal(&LCDFree);
    bx = data.x / 16;
    by = data.y / 16;
	  if (!BlockArray[bx][by].BlockUnoccupied.Value) {
			OS_bSignal(&BlockArray[bx][by].Touched);
			OS_WakeupThread(BlockArray[bx][by].threadId);
		}
//    BSP_LCD_Message(1, 5, 3, "X: ", x);
//    BSP_LCD_Message(1, 5, 12, "Y: ", y);
    ConsumerCount++;
    prevx = data.x;
    prevy = data.y;
  }
  OS_Kill(); // done
}

//--------------end of Task 3-----------------------------

//------------------Task 5--------------------------------
// UART background ISR performs serial input/output
// Two software fifos are used to pass I/O data to foreground
// The interpreter runs as a foreground thread
// inputs:  none
// outputs: none

void Interpreter(void) {
  char command[80];
  while (1) {
    OutCRLF();
    UART_OutString(">>");
    UART_InString(command, 79);
    OutCRLF();
    if (!(strcmp(command, "NumSamples"))) {
      UART_OutString("NumSamples: ");
      UART_OutUDec(NumSamples);
    } else if (!(strcmp(command, "MaxJitter"))) {
      UART_OutString("MaxJitter: ");
      UART_OutUDec(MaxJitter);
    } else if (!(strcmp(command, "DataLost"))) {
      UART_OutString("DataLost: ");
      UART_OutUDec(DataLost);
    } else if (!(strcmp(command, "UpdateWork"))) {
      UART_OutString("UpdateWork: ");
      UART_OutUDec(UpdateWork);
    } else if (!(strcmp(command, "Calculations"))) {
      UART_OutString("Calculations: ");
      UART_OutUDec(Calculation);
    } else if (!(strcmp(command, "FifoSize"))) {
      UART_OutString("JSFifoSize: ");
      UART_OutUDec(JSFIFOSIZE);
    } else if (!(strcmp(command, "Display"))) {
      UART_OutString("DisplayWork: ");
      UART_OutUDec(DisplayCount);
    } else if (!(strcmp(command, "Consumer"))) {
      UART_OutString("ConsumerWork: ");
      UART_OutUDec(ConsumerCount);
    } else {
      UART_OutString("Command incorrect!");
    }
  }
}
//--------------end of Task 5-----------------------------

//------------------Task 7--------------------------------
// background thread executes with button2
// one foreground task created with button push
// ***********ButtonWork2*************
void Restart(void) {
	NumSamples = RUNLENGTH;
	OS_AddThread(&ShotHandler, 128, 2);
	OS_Sleep(50);
	KillAllDemons();
	OS_Sleep(500);
	InitGameplay();
	JsFifo_Init();
	NumSamples = 0;
	OS_AddThread(&TitleScreenRoutine, 128, 3);
	OS_Kill();
}

//************SW2Push*************
// Called when Button2 pushed
// Adds another foreground task
// background threads execute once and return
void SW2Push(void) {
  Button2PushTime = OS_Time();
  if (OS_MsTime() > 20) { // debounce
    if (OS_AddThread(ReloadHandler, 128, 2)) {
      OS_ClearMsTime();
    }
    OS_ClearMsTime(); // at least 20ms between touches
  }
}

//--------------end of Task 7-----------------------------

// Fill the screen with the background color
// Grab initial joystick position to bu used as a reference
void CrossHair_Init(void) {
	uint8_t select;
  BSP_LCD_FillScreen(BGCOLOR);
  BSP_Joystick_Input(&origin[0], &origin[1], &select);
}

// All it does is eternally suspend itself to ensure that the scheduler doesn't crash.
void SuspendyThread(void) {
	while (1) OS_Suspend();
}


//******************* Main Function**********
int main(void) {
  OS_Init(); // initialize, disable interrupts
  Device_Init();
  CrossHair_Init();
	RenderInit();
	
	InitGameplay();
	
  DataLost = 0; // lost data between producer and consumer
  NumSamples = 0;
  MaxJitter = 0; // in 1us units

  //********initialize communication channels
  JsFifo_Init();

  //*******attach background tasks***********
  OS_AddSW1Task(&SW1Push, 4);
  OS_AddSW2Task(&SW2Push, 4);
  OS_AddPeriodicThread(&Producer, PERIOD, 3); // 2 kHz real time sampling of PD3

  // create initial foreground threads
//  OS_AddThread(&Interpreter, 128, 2);
	OS_AddThread(&RenderThread, 128, 2);
	OS_AddThread(&SuspendyThread, 128, 5);
	OS_AddThread(&TitleScreenRoutine, 128, 3);

  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
  return 0;            // this never executes
}
