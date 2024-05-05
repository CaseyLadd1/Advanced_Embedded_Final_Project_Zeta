// sound.c
// handles audio elements, contains song data
// Runs on LM4F120/TM4C123, as part of the MOOD MINI Project
// Sarah Hemler 4/25/24, seh5pp@virginia.edu
// Modified by John Berberian
// Modified by Paul Karhnak
// Modified by Casey Ladd

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "FIFO.h"
#include "sound.h"
#include "os.h"

static void InitPWM1();
static void tone_length(uint32_t duration);
static void play_tone(uint32_t frequency);
static void stop_buzzer();

#define G2		98
#define Gb2		104
#define A2		110
#define Ab2		117
#define B2		123
#define C3		131
#define Db3		139
#define D3		147
#define Eb3		156
#define E3		165
#define F3		175
#define Gb3		185
#define G3		196
#define Ab3		208
#define A3		220
#define Bb3		233
#define B3		247
#define C4		262
#define Db4		277
#define D4		294
#define Eb4		311
#define E4		330
#define F4		349
#define Gb4		370
#define G4		392
#define Ab4		415
#define A4		440
#define Bb4		466
#define B4		494
#define C5		523
#define Db5		554
#define D5		587
#define Eb5		622
#define E5		659
#define F5		698
#define Gb5		740
#define G5		784
#define Ab5		831
#define A5		880
#define Bb5		932
#define B5		988
#define C6		1047
#define Db6		1109
#define D6		1175
#define Eb6		1245
#define E6		1319
#define F6		1397
#define Gb6		1480
#define G6		1568
#define Ab6		1661
#define A6		1760
#define Bb6		1865
#define B6		1976

// Each element:
// {tone, duration, rest after}
// 
static uint16_t music[][3] = {
	{E3, 1, 1},
	{E3, 1, 1},
	{E4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{D4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{C4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{Bb3, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{B3, 1, 1},   
	{C4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{E4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{D4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{C4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{Bb3, 5, 6},
	
	{E3, 1, 1},
	{E3, 1, 1},
	{E4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{D4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{C4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{Bb3, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{B3, 1, 1},    
	{C4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{E4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 1},
	{D4, 1, 1},
	{E3, 1, 1},
	{E3, 1, 6},
	
	{A3, 1, 1},
	{A3, 1, 1},
	{A4, 1, 1},
	{A3, 1, 1},
	{A3, 1, 1},
	{G4, 1, 1},
	{A3, 1, 1},
	{A3, 1, 1},
	{F4, 1, 1},
	{A3, 1, 1},
	{A3, 1, 1},
	{Eb4, 1, 1},
	{A3, 1, 1},
	{A3, 1, 1},
	{E4, 1, 1},
	{F4, 1, 1},
	{A3, 1, 1},
	{A3, 1, 1},
	{A4, 1, 1},
	{A3, 1, 1},
	{A3, 1, 1},
	{G4, 1, 1},
	{A3, 1, 1},
	{A3, 1, 1},
	{F4, 1, 1},
	{A3, 1, 1},
	{A3, 1, 1},
	{Eb4, 5, 6},
	
	{Gb4, 1, 1},
	{E4, 1, 1},
	{Eb4, 1, 1},
	{Gb4, 1, 1},
	{A5, 1, 1},
	{G4, 1, 1},
	{Gb4, 1, 1},
	{Eb4, 1, 1},
	{Gb4, 1, 1},
	{G4, 1, 1},
	{A5, 1, 1},
	{B5, 1, 1},
	{A5, 1, 1},
	{G4, 1, 1},
	{Gb4, 1, 1},
	{Eb4, 1, 6},
	
	{B5, 1, 1},
	{G4, 1, 1},
	{E4, 1, 1},
	{G4, 1, 1},
	{B5, 1, 1},
	{G4, 1, 1},
	{B5, 1, 1},
	{E5, 1, 1},
	{B5, 1, 1},
	{G4, 1, 1},
	{B5, 1, 1},
	{G4, 1, 1},
	{B5, 1, 1},
	{E5, 1, 1},
	{G5, 1, 1},
	{B6, 1, 6}
};


static uint8_t BackgroundMusic;
static uint8_t tempo;
static uint32_t musicPosition;
static uint32_t trackLength = sizeof(music)/sizeof(uint16_t[3]);
static unsigned long soundThreadId;
static Sema4Type soundReady;
static Sema4Type eventSubmitted;

void InitSound(void) {
	BackgroundMusic = 0;
	SoundFifo_Init();
	InitPWM1();
	tempo = 100;
	musicPosition = 0;
	OS_InitSemaphore(&soundReady, 0);
	OS_InitSemaphore(&eventSubmitted, 0);
}

void AwaitSoundReady(void) {
	OS_bWait(&soundReady);
	OS_bSignal(&soundReady);
}


static void SubmitEvent(uint8_t n) {
	SoundFifo_Put((soundEvent) {
		.command = n
	});
	OS_bSignal(&eventSubmitted);
	OS_WakeupThread(soundThreadId);
}

void StartBackgroundMusic(void) {
	SubmitEvent(0);
}
void StopBackgroundMusic(void) {
	SubmitEvent(1);
}
void PointScoredSound(void) {
	SubmitEvent(2);
}
void LostLifeSound(void) {
	SubmitEvent(3);
}
void InitPWM1() {
	SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1; // Enable clock to PWM1 module
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // Enable system clock to PORTF
	SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // Enable clock division
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_PWMDIV_M); // Turn off direct feed
	SYSCTL_RCC_R |= SYSCTL_RCC_PWMDIV_4; // Turn on dividion by 4
	
	GPIO_PORTF_AFSEL_R |= (1 << 2); // PF2 set alternate function
	GPIO_PORTF_PCTL_R = GPIO_PCTL_PF2_M1PWM6; // Make PF2 PWM output pin
	GPIO_PORTF_DEN_R |= (1 << 2); // Set PF2 as digital pin
	
	PWM1_3_CTL_R &= ~(PWM_3_CTL_ENABLE); // Disable generator 3
	PWM1_3_CTL_R &= ~(PWM_3_CTL_MODE); // Select down count mode
	PWM1_3_GENA_R |= (1 << 2) | (1 << 3) | (1 << 7); // Set PWM output when counter reloads and clears when match
	PWM1_3_CTL_R |= PWM_3_CTL_ENABLE; // Enable generator 3
}

// Duration can be any number as long as the ratio between duration of notes is consistent
// If tempo is a requirement and a timer is necessary, put the number of milliseconds
static void play_tone(uint32_t frequency){
	uint32_t clock_frequency_with_divider = 16000000 / 4;
	uint32_t load_value = clock_frequency_with_divider / frequency;
	uint32_t duty_cycle = load_value / 2;
	
	PWM1_3_LOAD_R = load_value;
	PWM1_3_CMPA_R = duty_cycle;
	PWM1_ENABLE_R |= (1 << 6); // Enable PWM1 channel 6 output
}

static void stop_buzzer() {
	PWM1_ENABLE_R &= ~(1 << 6);
}

static void tone_length(uint32_t duration) {
	OS_Sleep((2 * duration - 1) * 125 * 60 / tempo);
}

void play_note(uint32_t note, uint32_t duration){
	play_tone(note);
	tone_length(duration);
}

static void play_rest(uint32_t duration){
	stop_buzzer();
	tone_length(duration);
}


static void _stepBackgroundMusic_internal(void) {
	play_note(music[musicPosition][0], music[musicPosition][1]);
	stop_buzzer();
	if (OS_bTry(&eventSubmitted)) return;
	 tone_length(music[musicPosition][2]);
}

static void _pointScored_internal(void) {
	play_rest(1);
	play_note(D4, 1);
	play_rest(1);
	play_note(D5, 1);
	play_rest(3);
}

static void _lifeLost_internal(void) {
	play_rest(1);
	play_note(E4, 1);
	play_rest(1);
	play_note(Eb4, 1);
	play_rest(1);
	play_note(D4, 1);
	play_rest(3);
}

void SoundThread(void) {
	soundEvent data;
	soundThreadId = OS_Id();
	OS_bSignal(&soundReady);
	while (1) {
		if (BackgroundMusic) {
			if (SoundFifo_TryGet(&data) == SOUNDFIFOFAIL) {
				OS_bTry(&eventSubmitted);
				_stepBackgroundMusic_internal();
				musicPosition++;
				musicPosition -= trackLength * (musicPosition == trackLength);
				continue;
			}
		} else {
			SoundFifo_Get(&data);
		}
		switch (data.command) {
			case 0:
				BackgroundMusic = 1;
				break;
			case 1:
				stop_buzzer();
			  BackgroundMusic = 0;
				break;
			case 2:
				_pointScored_internal();
				break;
			case 3:
				_lifeLost_internal();
				break;
			default:
				break;
		}
	}
}

//void E2M6(){
	//code goes here
	//PWM data for title track "Sinister"
	
	
//}

//void E1M1(){
	//code goes here
	//PWM data for main track "At Doom's Gate"
	
//main 1
	//E3 , 1/16
	//E3 , 1/16
	//E4 , 1/16
	//E3 , 1/16
	//E3 , 1/16
	//D4 , 1/16
	//E3 , 1/16
	//E3 , 1/16
	//C4 , 1/16
	//E3 , 1/16
	//E3 , 1/16
	//Bflat4 , 1/16
	//E3 , 1/16
	//E3 , 1/16
	//B4 , 1/16	
	//C4 , 1/16
	//E3 , 1/16
	//E4 , 1/16
	//E3 , 1/16
	//E3 , 1/16
	//D4 , 1/16
	//E3 , 1/16
	//E3 , 1/16
	//C4 , 1/16
	//E3 , 1/16
	//E3 , 1/16
	//Bflat4 , 5/16
//main 1
	
//alt main 1//
		//E3 , 1/16
		//E3 , 1/16
		//E4 , 1/16
		//E3 , 1/16
		//E3 , 1/16
		//D4 , 1/16
		//E3 , 1/16
		//E3 , 1/16
		//C4 , 1/16
		//E3 , 1/16
		//E3 , 1/16
		//Bb4 , 1/16
		//E3 , 1/16
		//E3 , 1/16
		//B4 , 1/16	
		//C4 , 1/16
		//E3 , 1/16
		//E3 , 1/16
		//E4 , 1/16
		//E3 , 1/16
		//E3 , 1/16
		//D4 , 1/16
		//E3 , 1/16
		//E3 , 1/16
//alt main 1//

//main 2
		//A4 , 1/16
		//A4 , 1/16
		//A5 , 1/16
		//A4 , 1/16
		//A4 , 1/16
		//G4 , 1/16
		//A4 , 1/16
		//A4 , 1/16
		//F4 , 1/16
		//A4 , 1/16
		//A4 , 1/16
		//Eb4 , 1/16
		//A4 , 1/16
		//A4 , 1/16
		//E4 , 1/16
		//F4 , 1/16
		//A4 , 1/16
		//A4 , 1/16
		//A5 , 1/16
		//A4 , 1/16
		//A4 , 1/16
		//G4 , 1/16
		//A4 , 1/16
		//A4 , 1/16
		//F4 , 1/16
		//A4 , 1/16
		//A4 , 1/16
		//Eb4 , 5/16
//main 2

//alt main 2
//alt main2

//main 3
//main 3

//riff 1
		//F#4 , 1/32
		//E4 , 1/32
		//D#4 , 1/32
		//F#4 , 1/32
		//A5 , 1/32
		//G4 , 1/32
		//F#4 , 1/32
		//D#4 , 1/32
		//F#4 , 1/32
		//G4 , 1/32
		//A5 , 1/32
		//B5 , 1/32
		//A5 , 1/32
		//G4 , 1/32
		//F#4 , 1/32
		//D#4 , 1/32
//riff 1

//riff 2
		//B5 , 1/32
		//G4 , 1/32
		//E4 , 1/32
		//G4 , 1/32
		//B5 , 1/32
		//G4 , 1/32
		//B5 , 1/32
		//E5 , 1/32
		//B5 , 1/32
		//G4 , 1/32
		//B5 , 1/32
		//G4 , 1/32
		//B5 , 1/32
		//E5 , 1/32
		//G5 , 1/32
		//B6 , 1/32
//riff 2

//riff 3
//riff 3
	
		//loop main 1 three times
		//alt main 1
		//riff 1
		//main
		//alt main 1
		//riff 2
		//main 2
		//alt main 2
		//riff 3
		//loop main 1 two times
		//main 3

	
//}

//void E2M3(){
	//code goes here
	//PWM data for game over track "Intermission From Doom"
//}