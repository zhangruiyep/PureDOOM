
#ifndef __DOOM_AUDIO_H__
#define __DOOM_AUDIO_H__

#define DOOM_AUDIO_SAMPLE_RATE 11025
#define DOOM_AUDIO_CHANNELS 2
#define DOOM_AUDIO_BITS 16

void doom_audio_init(void);
int doom_audio_open(int sample_rate, int channels, int bits);
void doom_audio_close(void);
void doom_audio_write(void* buffer, int size);

#endif
