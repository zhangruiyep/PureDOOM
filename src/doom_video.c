
#include <string.h>
#include "rtthread.h"
#include "doomdef.h"
#include "doom_mem.h"

// DOOM 256-color palette (set up by I_SetPalette during init)
extern unsigned char screen_palette[256 * 3];

// Precomputed BGR565 lookup table: palette index → 16-bit BGR565
static uint16_t pal_bgr565[256];
static int pal_ready = 0;

#define LOG_D rt_kprintf

unsigned short lcd_buffer[ SCREENWIDTH * SCREENHEIGHT ] __attribute__ ((aligned (4)));

unsigned int dwKeyPad1 = 0;

static rt_device_t g_lcd_device = RT_NULL;
static struct rt_device_graphic_info lcd_info;

static void build_bgr565_palette(void)
{
    for (int i = 0; i < 256; i++)
    {
        uint8_t r = screen_palette[i * 3 + 0];
        uint8_t g = screen_palette[i * 3 + 1];
        uint8_t b = screen_palette[i * 3 + 2];
        // CO5300 LCD 实际需要 RGB565 格式
        pal_bgr565[i] = (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3);
    }
    pal_ready = 1;
}

// Fast: palette indices → BGR565 via precomputed lookup table
static void pal_to_bgr565(const unsigned char *indices, uint16_t *out)
{
    int n = SCREENWIDTH * SCREENHEIGHT;
    for (int i = 0; i < n; i++)
        out[i] = pal_bgr565[indices[i]];
}

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

    // Fill entire LCD with black background (batched rows to reduce draw_rect calls)
    {
        int full_w = lcd_info.width;
        int full_h = lcd_info.height;
        int rows_per_batch = 16;
        int buf_pixels = full_w * rows_per_batch;
        uint16_t *black_buf = doom_mem_malloc(buf_pixels * 2);
        RT_ASSERT(black_buf);
        memset(black_buf, 0, buf_pixels * 2); // 0 = black in both RGB565 and BGR565
        rt_graphix_ops(g_lcd_device)->set_window(0, 0, full_w - 1, full_h - 1);
        for (int y = 0; y < full_h; y += rows_per_batch)
        {
            int rows = (full_h - y < rows_per_batch) ? full_h - y : rows_per_batch;
            rt_graphix_ops(g_lcd_device)->draw_rect((const char *)black_buf, 0, y, full_w - 1, y + rows - 1);
        }
        doom_mem_free(black_buf);
    }

    int32_t dx = (LCD_HOR_RES_MAX - SCREENWIDTH) / 2;
    int32_t dy = (LCD_VER_RES_MAX - SCREENHEIGHT) / 2;
    rt_graphix_ops(g_lcd_device)->set_window(dx, dy, dx + SCREENWIDTH - 1, dy + SCREENHEIGHT - 1);
    set_brightness(g_lcd_device);

    //rt_device_close(g_lcd_device);
}

void doom_video_refresh(const unsigned char *pal_indices)
{
    if (!pal_ready) build_bgr565_palette();

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

    // Direct palette → BGR565 (skips RGB888 intermediate)
    pal_to_bgr565(pal_indices, lcd_buffer);

    int32_t dx = (LCD_HOR_RES_MAX - SCREENWIDTH) / 2;
    int32_t dy = (LCD_VER_RES_MAX - SCREENHEIGHT) / 2;
    if (16 == lcd_info.bits_per_pixel)
    {
        rt_graphix_ops(g_lcd_device)->draw_rect((const char *)lcd_buffer, dx, dy, dx + SCREENWIDTH - 1, dy + SCREENHEIGHT - 1);
    }
    else if (24 == lcd_info.bits_per_pixel)
    {
        rt_graphix_ops(g_lcd_device)->draw_rect((const char *)pal_indices, dx, dy, dx + SCREENWIDTH - 1, dy + SCREENHEIGHT - 1);
    }
    else
    {
        rt_kprintf("%s %d: lcd bits_per_pixel %d not support\n", __func__, __LINE__, lcd_info.bits_per_pixel);
    }
}
