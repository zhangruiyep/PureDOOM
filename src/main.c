#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "doom_mem.h"
#include "doom_audio.h"
#include "DOOM.h"

extern void doom_video_init(void);
extern void doom_video_refresh(const unsigned char *pal_indices);

char *argv[2] = {"doom", "-shdev"};

/**
  * @brief  Main program
  * @param  None
  * @retval 0 if success, otherwise failure number
  */
int main(void)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t ms = 0;

    doom_video_init();
    doom_audio_init();
    doom_audio_open(DOOM_AUDIO_SAMPLE_RATE, DOOM_AUDIO_CHANNELS, DOOM_AUDIO_BITS);

    doom_set_malloc((doom_malloc_fn)doom_mem_malloc, (doom_free_fn)doom_mem_free);

    doom_init(sizeof(argv)/sizeof(argv[0]), argv, DOOM_FLAG_MENU_DARKEN_BG);

    while (1)
    {
        rt_uint32_t current = rt_tick_get();
        int fps = 1000/(current - ms);
        ms = current;
        rt_kprintf("fps: %d\n", fps);

        rt_uint32_t t0 = rt_tick_get();
        doom_update();
        rt_uint32_t t1 = rt_tick_get();

        const uint8_t* framebuffer = doom_get_framebuffer(1 /* palette indices */);

        doom_video_refresh(framebuffer);
        rt_uint32_t t2 = rt_tick_get();

        short* audio_buff = doom_get_sound_buffer();
        doom_audio_write(audio_buff, 2048);
        rt_uint32_t t3 = rt_tick_get();

        // Profile: print every 16 frames (~2 seconds)
        static int profile_cnt = 0;
        if (++profile_cnt == 16)
        {
            profile_cnt = 0;
            rt_kprintf("  [profile] doom_update:%dms  video_refresh:%dms  audio:%dms  total:%dms\n",
                t1 - t0, t2 - t1, t3 - t2, t3 - t0);
        }
    }

    doom_audio_close();
    return RT_EOK;

}
