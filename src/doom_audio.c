
#include "rtthread.h"
#include <audio_server.h>

#define LOG_D rt_kprintf

static audio_client_t g_speaker = NULL;

void doom_audio_init(void)
{
    g_speaker = NULL;
    audio_server_set_private_volume(AUDIO_TYPE_LOCAL_MUSIC, 9);//设置音量
}

int doom_audio_open(int sample_rate, int channels, int bits)
{
    if (!g_speaker)
    {
        LOG_D("%s: %d %d %d", __func__, sample_rate, channels, bits);
        audio_parameter_t pa = {0};
        pa.write_bits_per_sample = bits;
        pa.write_channnel_num = channels;
        pa.write_samplerate = sample_rate;
        pa.read_bits_per_sample = bits;
        pa.read_channnel_num = channels;
        pa.read_samplerate = sample_rate;
        pa.read_cache_size = 0;
        pa.write_cache_size = 16000;
        g_speaker = audio_open(AUDIO_TYPE_LOCAL_MUSIC, AUDIO_TX, &pa, NULL, NULL);
        if (!g_speaker)
        {
            LOG_D("audio_open failed\n");
            return -1;
        }
    }
    return 0;
}

void doom_audio_close( void )
{
    if (g_speaker)
    {
        LOG_D("%s", __func__);
        uint32_t cache_time_ms = 150;
        audio_ioctl(g_speaker, 1, &cache_time_ms);
        rt_thread_mdelay(cache_time_ms + 20);
        audio_close(g_speaker);
        g_speaker = NULL;
    }
}

int doom_audio_write(uint8_t *data, uint32_t data_len)
{
    if (!g_speaker)
    {
        LOG_D("%s: not open", __func__);
        return 0;
    }
    //LOG_D("%s: write %d", __func__, data_len);
    return audio_write(g_speaker, data, data_len);
}
