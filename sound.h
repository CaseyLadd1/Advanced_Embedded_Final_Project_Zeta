#ifndef EMB_SOUND_H
#define EMB_SOUND_H

void InitSound(void);
void AwaitSoundReady(void);
void SoundThread(void);

void StartBackgroundMusic(void);
void StopBackgroundMusic(void);
void PointScoredSound(void);
void LostLifeSound(void);

#endif