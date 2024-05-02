/*
Created on 28 april 2024
By Sarah Hemler

Based off Esplora Music code:  
  Created on 22 november 2012
  By Enrico Gueli <enrico.gueli@gmail.com>
  modified 22 Dec 2012
  by Tom Igoe
*/


#include <Esplora.h>

// these are the frequencies for the notes from middle C
// to one octave above middle C:
  int current1=0;
  int current2=0;
  int current3=0;
  int current4=0;
  int i=0;
  const int tonePin = 8;
  unsigned long previousMillis = 0;
  boolean outputTone = false; 
  
const int note[] = {
  262, // C
  277, // C#
  294, // D
  311, // D#
  330, // E
  349, // F
  370, // F#
  392, // G
  415, // G#
  440, // A
  466, // A#
  494, // B
  523  // C next octave
};

  //PWM data for main track "At Doom's Gate"
  
const int main1[]={ //main 1
165,  //E3 , 1/16
165,  //E3 , 1/16
330,  //E4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
294,  //D4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
262,  //C4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
233,  //Bflat4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
247,  //B4 , 1/16 
262,  //C4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
330,  //E4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
294,  //D4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
262,  //C4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
233};  //Bflat4 , 5/16
//main 1

const int main1dur[]={
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
250
};

//const int main1dur[]={
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//500
//};
  
const int altmain1[]={//alt main 1//
165,  //E3 , 1/16
165,  //E3 , 1/16
330,  //E4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
294,  //D4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
262,  //C4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
233,  //Bflat4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
247,  //B4 , 1/16 
262,  //C4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
330,  //E4 , 1/16
165,  //E3 , 1/16
165,  //E3 , 1/16
294,  //D4 , 1/16
165,  //E3 , 1/16
165  //E3 , 1/16};
//alt main 1//
};

const int main1altdur[]={
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
50,
250
};

//const int main1altdur[]={
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//100,
//500
//};
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

const int riff1[]={//riff 1
370,    //F#4 , 1/32
330,    //E4 , 1/32
311,    //D#4 , 1/32
370,    //F#4 , 1/32
440,    //A5 , 1/32
392,    //G4 , 1/32
370,    //F#4 , 1/32
311,    //D#4 , 1/32
370,    //F#4 , 1/32
392,    //G4 , 1/32
440,    //A5 , 1/32
494,    //B5 , 1/32
440,    //A5 , 1/32
392,    //G4 , 1/32
370,    //F#4 , 1/32
311    //D#4 , 1/32
//riff 1
};

//const int riffdur1[]={
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50,
//50  
//};

const int riffdur1[]={
25,
25,
25,
25,
25,
25,
25,
25,
25,
25,
25,
25,
25,
25,
25,
25  
};


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


void setup() {
}


void loop() {
  int current=0;

  // read the button labeled SWITCH_DOWN. If it's low,
  while(1){
  int current = 0;  
while(current<(sizeof(main1)/sizeof(main1[0]))){
    Esplora.tone(main1[current], main1dur[current]);
    int pauseBetweenNotes = main1dur[current];
    unsigned long currentMillis = millis();



if (outputTone) {
// We are currently outputting a tone
// Check if it's been long enough and turn off if so
    if (currentMillis - previousMillis >= main1dur[current]) {
        previousMillis = currentMillis;
        Esplora.noTone();
        outputTone = false;
    }
} else {
// We are currently in a pause
// Check if it's been long enough and turn on if so
    if (currentMillis - previousMillis >= pauseBetweenNotes) {
        previousMillis = currentMillis;
        
        tone(main1[current],main1dur[current]);
        outputTone = true;
        current++;
    }
}
}
current=0;
while(current<(24)){
    Esplora.tone(altmain1[current], main1altdur[current]);
    int pauseBetweenNotes = main1dur[current];
    unsigned long currentMillis = millis();
if (outputTone) {
// We are currently outputting a tone
// Check if it's been long enough and turn off if so
    if (currentMillis - previousMillis >= main1altdur[current]) {
        previousMillis = currentMillis;
        Esplora.noTone();
        outputTone = false;
    }
} else {
// We are currently in a pause
// Check if it's been long enough and turn on if so
    if (currentMillis - previousMillis >= pauseBetweenNotes) {
        previousMillis = currentMillis;
        
        tone(altmain1[current],main1altdur[current]);
        outputTone = true;
        current++;

    }
}  
}
current=0;
while(current<(16)){
    Esplora.tone(riff1[current], riffdur1[current]);
    int pauseBetweenNotes = riffdur1[current];
    unsigned long currentMillis = millis();
if (outputTone) {
// We are currently outputting a tone
// Check if it's been long enough and turn off if so
    if (currentMillis - previousMillis >= riffdur1[current]) {
        previousMillis = currentMillis;
        Esplora.noTone();
        outputTone = false;
    }
} else {
// We are currently in a pause
// Check if it's been long enough and turn on if so
    if (currentMillis - previousMillis >= pauseBetweenNotes) {
        previousMillis = currentMillis;
        
        tone(riff1[current],riffdur1[current]);
        outputTone = true;
        current++;

    }
}  
}
}

}

void main1tune(){
     if(current1>=(sizeof(main1)/sizeof(main1[0]))){
      current1=0;
    }
    if (current1<((sizeof(main1)/sizeof(main1[0]))-1)){
      Esplora.tone(main1[current1],50);
    }
    else{
      Esplora.tone(main1[current1],550);
    }
   if (current1<((sizeof(main1)/sizeof(main1[0]))-1)){
      delay(50);
    }
    else{
      delay(650);
    }
    Esplora.noTone();
    delay(100);
    current1++;
    i++;
  }

 void main1alt(){
    if(current2>=(sizeof(altmain1)/sizeof(altmain1[0]))){
      current2=0;
    }
      Esplora.tone(altmain1[current2],50);
      delay(50);
    Esplora.noTone();
    delay(100);
    current2++;
    i++;
  }

 void riffOne(){
    if(current3>=(sizeof(riff1)/sizeof(riff1[0]))){
      current3=0;
    }
      Esplora.tone(riff1[current3],25);
      delay(25);
    Esplora.noTone();
    delay(100);
    current3++;
    i++;
  }
