// sound.c
// handles audio elements, contains song data
// Runs on LM4F120/TM4C123, as part of the MOOD MINI Project
// Sarah Hemler 4/25/24, seh5pp@virginia.edu
// Modified by John Berberian
// Modified by Paul Karhnak
// Modified by Casey Ladd

#include "tm4c123gh6pm.h"
#include <stdint.h>

#define tempo 100
void InitTimer4A();
void InitPWM1();
void tone_length(uint32_t duration);
void play_tone(uint32_t frequency);
void stop_buzzer();
void play_E1M1_demo();
void play_E1M1_main1();
void play_E1M1_altmain1();
void play_E1M1_main2();
void play_E1M1_riff1();
void play_E1M1_riff2();

#define NVIC_EN2_INT70 0x00000040

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

volatile uint32_t ms_delay = 0;

int main() {
	InitPWM1();
	InitTimer4A();
	play_E1M1_demo();
	while(1) {}
}

void InitTimer4A() {
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R4;
	while ((SYSCTL_RCGCTIMER_R & 0x10) == 0) {} // allow time for clock to stabilize
	TIMER4_CTL_R &= ~TIMER_CTL_TAEN; // disable timer4A during setup
	TIMER4_CFG_R = TIMER_CFG_32_BIT_TIMER;  // configure for 32-bit timer mode
	TIMER4_TAMR_R = TIMER_TAMR_TAMR_PERIOD; // configure for periodic mode, default down-count settings
	TIMER4_TAILR_R = 16000 - 1; // reload value           
	TIMER4_ICR_R = TIMER_ICR_TATOCINT; // clear timer4A timeout flag
	TIMER4_IMR_R |= TIMER_IMR_TATOIM; // arm timeout interrupt
	NVIC_PRI17_R = (NVIC_PRI17_R & 0xFF00FFFF) | (3 << 21); // 3 // // priority shifted to bits 15-13 for timer1A
	NVIC_EN2_R = NVIC_EN2_INT70; // enable interrupt 70 in NVIC
	TIMER4_TAPR_R = 0;
	TIMER4_CTL_R |= TIMER_CTL_TAEN; // enable timer4A
}

void Timer4A_Handler(void) {
	TIMER4_ICR_R = TIMER_ICR_TATOCINT; // acknowledge timer4A timeout
	ms_delay++;
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
//save tracks as arrays or TCBs?
// Commented out for compiler version 6 to work.
/*
void play_track(track){
	//code goes here
	//creates a thread that plays a given track
}
*/

void play_note(uint32_t note, uint32_t duration){
	play_tone(note);
	tone_length(duration);
	stop_buzzer();
	tone_length(1);
}

void play_rest(uint32_t duration){
	stop_buzzer();
	tone_length(duration);
	tone_length(1);
}

// Duration can be any number as long as the ratio between duration of notes is consistent
// If tempo is a requirement and a timer is necessary, put the number of milliseconds
void play_tone(uint32_t frequency){
	uint32_t clock_frequency_with_divider = 16000000 / 4;
	uint32_t load_value = clock_frequency_with_divider / frequency;
	uint32_t duty_cycle = load_value / 2;
	
	PWM1_3_LOAD_R = load_value;
	PWM1_3_CMPA_R = duty_cycle;
	PWM1_ENABLE_R |= (1 << 6); // Enable PWM1 channel 6 output
}

void tone_length(uint32_t duration) {
	uint32_t milliseconds = (duration * 1000 * 60 / tempo / 4) - (1000 * 60 / tempo / 8);

	uint32_t start_time = ms_delay;
  	while ((ms_delay - start_time) < milliseconds) {}
	ms_delay = 0;
}

void stop_buzzer() {
	PWM1_ENABLE_R &= ~(1 << 6);
}

void play_E1M1_demo(){
	play_E1M1_main1();
	tone_length(4);
	play_E1M1_altmain1();
	tone_length(4);
	play_E1M1_main2();
	tone_length(4);
	play_E1M1_riff1();
	tone_length(4);
	play_E1M1_riff2();
}

void play_E1M1_main1(){
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(E4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(D4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(C4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(Bb3, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(B3, 1);   
	play_note(C4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(E4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(D4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(C4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(Bb3, 5);

}

void play_E1M1_altmain1(){
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(E4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(D4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(C4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(Bb3, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(B3, 1);    
	play_note(C4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(E4, 1);
	play_note(E3, 1);
	play_note(E3, 1);
	play_note(D4, 1);
	play_note(E3, 1);
	play_note(E3, 1);

}

void play_E1M1_main2(){
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(A4, 1);
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(G4, 1);
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(F4, 1);
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(Eb4, 1);
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(E4, 1);
	play_note(F4, 1);
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(A4, 1);
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(G4, 1);
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(F4, 1);
	play_note(A3, 1);
	play_note(A3, 1);
	play_note(Eb4, 5);

}

void play_E1M1_riff1(){
	play_note(Gb4, 1);
	play_note(E4, 1);
	play_note(Eb4, 1);
	play_note(Gb4, 1);
	play_note(A5, 1);
	play_note(G4, 1);
	play_note(Gb4, 1);
	play_note(Eb4, 1);
	play_note(Gb4, 1);
	play_note(G4, 1);
	play_note(A5, 1);
	play_note(B5, 1);
	play_note(A5, 1);
	play_note(G4, 1);
	play_note(Gb4, 1);
	play_note(Eb4, 1);

}

void play_E1M1_riff2(){
	play_note(B5, 1);
	play_note(G4, 1);
	play_note(E4, 1);
	play_note(G4, 1);
	play_note(B5, 1);
	play_note(G4, 1);
	play_note(B5, 1);
	play_note(E5, 1);
	play_note(B5, 1);
	play_note(G4, 1);
	play_note(B5, 1);
	play_note(G4, 1);
	play_note(B5, 1);
	play_note(E5, 1);
	play_note(G5, 1);
	play_note(B6, 1);

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