// sound.c
// handles audio elements, contains song data
// Runs on LM4F120/TM4C123, as part of the MOOD MINI Project
// Sarah Hemler 4/25/24, seh5pp@virginia.edu
// Modified by John Berberian
// Modified by Paul Karhnak
// Modified by Casey Ladd

//Original code to implemenent only one frequency
/*
	SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_USEPWMDIV);
	
	GPIO_PORTF_AFSEL_R |= (1 << 2);
	GPIO_PORTF_PCTL_R = GPIO_PCTL_PF2_M1PWM6;
	GPIO_PORTF_DEN_R |= (1 << 2);
	
	PWM1_3_CTL_R &= ~(PWM_3_CTL_ENABLE);
	PWM1_3_CTL_R &= ~(PWM_3_CTL_MODE);
	PWM1_3_LOAD_R = load_value;
	PWM1_3_CMPA_R = duty_cycle;
	PWM1_3_GENA_R |= (1 << 2) | (1 << 3) | (1 << 7); 
	PWM1_3_CTL_R |= PWM_3_CTL_ENABLE;
	PWM1_ENABLE_R |= (1 << 6);
	*/

#include "tm4c123gh6pm.h"
#include <stdint.h>
//#include <string.h>

// #define tempo		// Might need to use a timer to use but not necessary

int initialize_FWM();
int play_tone(uint32_t frequency, uint32_t duration);

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


int main() {
	initialize_FWM();
	uint32_t duration = 10000000;
	
	play_tone(G2,duration);
	
	play_tone(Gb2,duration);
	play_tone(A2,duration);
	play_tone(Ab2,duration);
	play_tone(B2,duration);
	play_tone(C3,duration);
	play_tone(Db3,duration);
	play_tone(D3,duration);
	play_tone(Eb3,duration);
	play_tone(E3,duration);
	play_tone(F3,duration);
	play_tone(Gb3,duration);
	play_tone(G3,duration);
	play_tone(Ab3,duration);
	play_tone(A3,duration);
	play_tone(Bb3,duration);
	play_tone(B3,duration);
	play_tone(C4,duration);
	play_tone(Db4,duration);
	play_tone(D4,duration);
	play_tone(Eb4,duration);
	play_tone(E4,duration);
	play_tone(F4,duration);
	play_tone(Gb4,duration);
	play_tone(G4,duration);
	play_tone(Ab4,duration);
	play_tone(A4,duration);
	play_tone(Bb4,duration);
	play_tone(B4,duration);
	play_tone(C5,duration);
	play_tone(Db5,duration);
	play_tone(D5,duration);
	play_tone(Eb5,duration);
	play_tone(E5,duration);
	play_tone(F5,duration);
	play_tone(Gb5,duration);
	play_tone(G5,duration);
	play_tone(Ab5,duration);
	play_tone(A5,duration);
	
	while(1) {
	}
}


int initialize_FWM() {
	SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	SYSCTL_RCC_R &= ~(SYSCTL_RCC_USEPWMDIV);
	
	GPIO_PORTF_AFSEL_R |= (1 << 2);
	GPIO_PORTF_PCTL_R = GPIO_PCTL_PF2_M1PWM6;
	GPIO_PORTF_DEN_R |= (1 << 2);
	
	PWM1_3_CTL_R &= ~(PWM_3_CTL_ENABLE);
	PWM1_3_CTL_R &= ~(PWM_3_CTL_MODE);
	PWM1_3_GENA_R |= (1 << 2) | (1 << 3) | (1 << 7); 
	PWM1_3_CTL_R |= PWM_3_CTL_ENABLE;
	PWM1_ENABLE_R |= (1 << 6);
	
	return 0;
} 
//save tracks as arrays or TCBs?
// Commented out for compiler version 6 to work.
/*
void play_track(track){
	//code goes here
	//creates a thread that plays a given track
}
*/

// Duration can be any number as long as the ratio between duration of notes is consistent
// If tempo is a requirement and a timer is necessary, put the number of milliseconds
int play_tone(uint32_t frequency, uint32_t duration){
	uint32_t load_value = 16000000 / frequency;
	uint32_t duty_cycle = load_value;
	
	PWM1_3_CTL_R &= ~(PWM_3_CTL_ENABLE);
	PWM1_3_LOAD_R = load_value;
	PWM1_3_CMPA_R = duty_cycle;
	PWM1_3_CTL_R |= PWM_3_CTL_ENABLE;
	PWM1_ENABLE_R |= (1 << 6);
	uint32_t playtime = 0;
	while(playtime < duration){
		playtime++;
	}
	return 0;
	// Immediately call the next note or end the song by calling end song
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