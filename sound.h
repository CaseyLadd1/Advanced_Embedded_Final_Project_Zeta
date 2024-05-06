#ifndef EMB_SOUND_H
#define EMB_SOUND_H

void InitSound(void);
void AwaitSoundReady(void);
void SoundThread(void);

void StartBackgroundMusic(void);
void StopBackgroundMusic(void);
void PointScoredSound(void);
void LostLifeSound(void);

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

#endif