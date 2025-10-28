#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "dfs_file.h"
#include "spi_msd.h"

int mnt_init(void)
{
    uint16_t time_out = 100;
    while (time_out --)
    {
        rt_thread_mdelay(30);
        if (rt_device_find("sd0"))
            break;
    }
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0) // fs exist
    {
        rt_kprintf("mount fs on flash to root success\n");
    }
    else
    {
        rt_kprintf("dfs_mkfs elm flash fail\n");
    }

    return RT_EOK;
}
INIT_ENV_EXPORT(mnt_init);


#include "bf0_hal_aon.h"
rt_uint32_t *buff_test = (rt_uint32_t *)(0x60000000);

#define SDIO_TEST_LEN 1024 * 1024
void cmd_fs_write_t(char *path, int num)
{
    struct dfs_fd fd_test_sd;
    uint32_t open_time = 0, end_time = 0;
    float test_time = 0.0;
    float speed_test = 0.0;
    //char *buff = app_malloc(SDIO_TEST_LEN);
    memset(buff_test, 0x55, SDIO_TEST_LEN);
    uint32_t write_num = num;
    uint32_t write_byt = write_num * SDIO_TEST_LEN * 8;
    if (dfs_file_open(&fd_test_sd, path, O_RDWR | O_CREAT | O_TRUNC) == 0)
    {
        open_time = HAL_GTIMER_READ();
        while (write_num--)
        {
            dfs_file_write(&fd_test_sd, buff_test, SDIO_TEST_LEN);
        }
        end_time = HAL_GTIMER_READ();
    }
    dfs_file_close(&fd_test_sd);
    test_time = ((end_time - open_time) / HAL_LPTIM_GetFreq()) * 1000 * 1000;
    speed_test = write_byt / test_time;
    rt_kprintf("%s path=%s num=%d b testtime=%.6lfuS,speed_test=%.6lfMb/s\n", __func__, path, num, test_time, speed_test);
    //app_free(buff);

}

void cmd_fs_write(int argc, char **argv)
{
    cmd_fs_write_t(argv[1], atoi(argv[2]));

}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_fs_write, __cmd_fs_write, test write speed);

void cmd_fs_read_t(char *path, int num)
{
    struct dfs_fd fd_read;
    uint32_t open_time = 0, end_time = 0;
    float test_time = 0.0;
    float speed_test = 0.0;
    //char *buff = app_malloc(SDIO_TEST_LEN);
    uint32_t read_num = num;
    uint32_t read_byt = read_num * SDIO_TEST_LEN * 8;
    rt_memset(buff_test, 0, SDIO_TEST_LEN);
    if (dfs_file_open(&fd_read, path, O_RDONLY) == 0)
    {
        open_time = HAL_GTIMER_READ();
        while (read_num)
        {
            dfs_file_read(&fd_read, buff_test, SDIO_TEST_LEN);
            read_num--;
        }
        end_time = HAL_GTIMER_READ();
    }
    dfs_file_close(&fd_read);
    test_time = ((end_time - open_time) / HAL_LPTIM_GetFreq()) * 1000 * 1000;
    speed_test = read_byt / test_time;
    rt_kprintf("%s  path=%s num=%d b testtime=%.6lfuS,speed_test=%.6lfMb/s\n", __func__, path, num, test_time, speed_test);
    //app_free(buff);
}

void cmd_fs_read(int argc, char **argv)
{
    cmd_fs_read_t(argv[1], atoi(argv[2]));
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_fs_read, __cmd_fs_read, test read speed);

int cmd_emmc_test_buff(int argc, char **argv)
{
    if (atoi(argv[1]) == 512)
        buff_test = (rt_uint32_t *)(0x60000000);//
    else if (atoi(argv[1]) == 1024)
        buff_test = (rt_uint32_t *)(0x60100000);//1M
    else buff_test = (rt_uint32_t *)(0x60000003);
    rt_kprintf("%s buff_test=%p\n", __func__, buff_test);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_emmc_test_buff, __cmd_emmc_test_buff, cmd emmc tes buff);
