
#include <string.h>
#include "rtthread.h"
#include "doomdef.h"
#include "doom_mem.h"

#define LOG_D rt_kprintf

unsigned short lcd_buffer[ SCREENWIDTH * SCREENHEIGHT ] __attribute__ ((aligned (4)));
//extern void start_application( void );
//extern void close_application( void );

unsigned int dwKeyPad1 = 0;

static rt_device_t g_lcd_device = RT_NULL;
static struct rt_device_graphic_info lcd_info;

static void set_brightness(rt_device_t lcd_device)
{
    //rt_err_t err = rt_device_open(lcd_device, RT_DEVICE_OFLAG_RDWR);
    //if ((RT_EOK == err) || (-RT_EBUSY == err))
    {
        uint8_t brightness = 100;
        rt_device_control(lcd_device, RTGRAPHIC_CTRL_SET_BRIGHTNESS, &brightness);

        //if (RT_EOK == err) rt_device_close(lcd_device);
    }
}

void doom_video_init(void)
{
    /* use lcd device api instead of littlevGL */
    rt_err_t err;
    g_lcd_device = rt_device_find("lcd");
    if (!g_lcd_device)
    {
        rt_kprintf("Can't find lcd\n");
        return;
    }

    err = rt_device_open(g_lcd_device, RT_DEVICE_OFLAG_RDWR);
    if (RT_EOK != err)
    {
        rt_kprintf("lcd open err %d\n", err);
        return;
    }

    if (rt_device_control(g_lcd_device, RTGRAPHIC_CTRL_GET_INFO, &lcd_info) == RT_EOK)
    {
        rt_kprintf("Lcd info w:%d, h%d, bits_per_pixel %d\r\n", lcd_info.width, lcd_info.height, lcd_info.bits_per_pixel);
    }

    uint16_t cf;
    if (16 == lcd_info.bits_per_pixel)
        cf = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    else if (24 == lcd_info.bits_per_pixel)
        cf = RTGRAPHIC_PIXEL_FORMAT_RGB888;
    else
        RT_ASSERT(0);

    rt_device_control(g_lcd_device, RTGRAPHIC_CTRL_SET_BUF_FORMAT, &cf);

#if 0
    char *lcd_bg_bufer = doom_mem_malloc(SCREENWIDTH * SCREENHEIGHT * 2);
    RT_ASSERT(lcd_bg_bufer);
    memset(lcd_bg_bufer, 0, SCREENWIDTH * SCREENHEIGHT * 2);
    rt_graphix_ops(g_lcd_device)->draw_rect((const char *)lcd_bg_bufer, 0, 0, SCREENWIDTH - 1, SCREENHEIGHT - 1);
    doom_mem_free(lcd_bg_bufer);
#endif

    int32_t dx = (LCD_HOR_RES_MAX - SCREENWIDTH) / 2;
    int32_t dy = (LCD_VER_RES_MAX - SCREENHEIGHT) / 2;
    rt_graphix_ops(g_lcd_device)->set_window(dx, dy, dx + SCREENWIDTH - 1, dy + SCREENHEIGHT - 1);
    set_brightness(g_lcd_device);

    //rt_device_close(g_lcd_device);
}

static void doom_rgb888_to_rgb565(uint8_t *rgb888, uint16_t *rgb565)
{
    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++)
    {
        uint8_t r = rgb888[i * 3];
        uint8_t g = rgb888[i * 3 + 1];
        uint8_t b = rgb888[i * 3 + 2];
        rgb565[i] = (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3);
    }
}

void doom_video_refresh(uint8_t *rgb888)
{
    if (!g_lcd_device)
    {
        rt_kprintf("%s %d: no lcd dev\n", __func__, __LINE__);
        return;
    }

    if (!g_lcd_device->user_data)
    {
        rt_kprintf("%s %d: lcd no ops\n", __func__, __LINE__);
        return;
    }

    if (16 == lcd_info.bits_per_pixel)
    {
        doom_rgb888_to_rgb565(rgb888, lcd_buffer);
    }

#if 0
    // test
    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++)
    {
        rgb888[i * 3] = 0x00;
        rgb888[i * 3 + 1] = 0x00;
        rgb888[i * 3 + 2] = 0xFF;
    }
#endif

    //rt_kprintf("%s %d\n", __func__, __LINE__);
    int32_t dx = (LCD_HOR_RES_MAX - SCREENWIDTH) / 2;
    int32_t dy = (LCD_VER_RES_MAX - SCREENHEIGHT) / 2;
    if (16 == lcd_info.bits_per_pixel)
    {
        rt_graphix_ops(g_lcd_device)->draw_rect((const char *)lcd_buffer, dx, dy, dx + SCREENWIDTH - 1, dy + SCREENHEIGHT - 1);
    }
    else if (24 == lcd_info.bits_per_pixel)
    {
        rt_graphix_ops(g_lcd_device)->draw_rect((const char *)rgb888, dx, dy, dx + SCREENWIDTH - 1, dy + SCREENHEIGHT - 1);
    }
    else
    {
        rt_kprintf("%s %d: lcd bits_per_pixel %d not support\n", __func__, __LINE__, lcd_info.bits_per_pixel);
    }
    //rt_kprintf("%s %d\n", __func__, __LINE__);
}
