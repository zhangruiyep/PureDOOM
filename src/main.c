#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "doom_mem.h"
#include "DOOM.h"

char *argv[1] = {"main"};

/**
  * @brief  Main program
  * @param  None
  * @retval 0 if success, otherwise failure number
  */
int main(void)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t ms;

    doom_set_malloc((doom_malloc_fn)doom_mem_malloc, (doom_free_fn)doom_mem_free);

    doom_init(1, argv, DOOM_FLAG_MENU_DARKEN_BG);

    while (1)
    {
        doom_update();
        rt_thread_mdelay(1);
    }
    return RT_EOK;

}
