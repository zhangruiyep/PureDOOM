#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "doom_mem.h"
#include "DOOM.h"

extern void doom_video_init(void);
extern void doom_video_refresh(uint8_t *rgb888);

char *argv[2] = {"doom", "-shdev"};

/**
  * @brief  Main program
  * @param  None
  * @retval 0 if success, otherwise failure number
  */
int main(void)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t ms;

    doom_video_init();

    doom_set_malloc((doom_malloc_fn)doom_mem_malloc, (doom_free_fn)doom_mem_free);

    doom_init(sizeof(argv)/sizeof(argv[0]), argv, DOOM_FLAG_MENU_DARKEN_BG);

    while (1)
    {
        doom_update();
        uint8_t* framebuffer = doom_get_framebuffer(3 /* RGB */);
        doom_video_refresh(framebuffer);
        //rt_thread_mdelay(1);
    }
    return RT_EOK;

}
