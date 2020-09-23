/*************************************************
Copyright (c) 2020
All rights reserved.
File name:     filesystem.c
Description:   
History:
1. Version:    V1.0.0
Date:      2020-09-06
Author:    WKJay
Modify:    
*************************************************/
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <fal.h>
#include <stdio.h>

#define FS_MOUNT_MAX_RETRY 3

#define DBG_TAG "fs"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static int fs_is_ready = 0;
extern int stm32_sdcard_mount(void);

int filesystem_init(void)
{
    uint8_t retry = 0;
    struct rt_device *flash_dev = fal_blk_device_create("filesystem");
_spiflash_mount_retry:
    if (dfs_mount(flash_dev->parent.name, "/", "elm", 0, 0) != 0)
    {
        rt_kprintf("norflash mount failed!\n");
        dfs_mkfs("elm", flash_dev->parent.name);
        if (retry == FS_MOUNT_MAX_RETRY)
            return -1;

        retry++;
        goto _spiflash_mount_retry;
    }
    else
    {
        stm32_sdcard_mount();
    }
    fs_is_ready = 1;
    return 0;
}

int root_filesystem_is_ready(void)
{
    return fs_is_ready;
}

char *fs_read_file(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        LOG_E("cannot find config file: %s!", path);
        return RT_NULL;
    }

    fseek(fp, 0, SEEK_END);
    int fp_size = ftell(fp);
    rewind(fp);
    LOG_D("file size is %d", fp_size);
    if(fp_size <= 0)
    {
        fclose(fp);
        return RT_NULL;
    }

    char *buf = rt_malloc(fp_size + 1);
    if(buf == RT_NULL)
    {
        fclose(fp);
        return RT_NULL;
    }

    rt_memset(buf, 0, fp_size + 1);
    int result = fread(buf, fp_size, 1, fp);
    buf[fp_size] = 0;

    if (result != 1)
    {
        LOG_E("file read error");
        rt_free(buf);
        fclose(fp);
        return RT_NULL;
    }

    fclose(fp);
    return buf;
}
RTM_EXPORT(fs_read_file);

int fs_create_dir(const char *path)
{
    int result = RT_EOK;

    DIR *dir = opendir(path);
    if(dir == RT_NULL)
    {
        LOG_I("create %s dir.", path);
        if(mkdir(path, 0x777) != 0)
            result = -RT_ERROR;
    }
    else
        closedir(dir);

    return result;
}
RTM_EXPORT(fs_create_dir);

char* fs_normalize_path(char* fullpath)
{
    char *dst0, *dst, *src;

    src = fullpath;
    dst = fullpath;

    dst0 = dst;
    while (1)
    {
        char c = *src;

        if (c == '.')
        {
            if (!src[1]) src ++; /* '.' and ends */
            else if (src[1] == '/')
            {
                /* './' case */
                src += 2;

                while ((*src == '/') && (*src != '\0')) src ++;
                continue;
            }
            else if (src[1] == '.')
            {
                if (!src[2])
                {
                    /* '..' and ends case */
                    src += 2;
                    goto up_one;
                }
                else if (src[2] == '/')
                {
                    /* '../' case */
                    src += 3;

                    while ((*src == '/') && (*src != '\0')) src ++;
                    goto up_one;
                }
            }
        }

        /* copy up the next '/' and erase all '/' */
        while ((c = *src++) != '\0' && c != '/') *dst ++ = c;

        if (c == '/')
        {
            *dst ++ = '/';
            while (c == '/') c = *src++;

            src --;
        }
        else if (!c) break;

        continue;

up_one:
        dst --;
        if (dst < dst0) return RT_NULL;
        while (dst0 < dst && dst[-1] != '/') dst --;
    }

    *dst = '\0';

    /* remove '/' in the end of path if exist */
    dst --;
    if ((dst != fullpath) && (*dst == '/')) *dst = '\0';

    return fullpath;
}
