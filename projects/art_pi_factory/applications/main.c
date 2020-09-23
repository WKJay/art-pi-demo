/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-02     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"
#include <netdev_ipaddr.h>
#include <ntp.h>
#include <netdev.h>

#include "web.h"
#include "basic.h"
#include "filesystem.h"
#include "monitor.h"
#include "bt_module.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern int ftp_init(void);

int main(void)
{
    filesystem_init();
    basic_init();
    sys_monitor_init();
    bluetooth_init();
    web_init();
    ftp_init();

    while (1)
    {
        rt_thread_mdelay(1000);
    }
    //    while (ntp_sync_to_rtc(NULL) == 0)
    //    {
    //        rt_thread_mdelay(5000);
    //    }
    //    LOG_I("NTP sync success");

    return RT_EOK;
}

#include "stm32h7xx.h"
static int vtor_config(void)
{
    /* Vector Table Relocation in Internal QSPI_FLASH */
    SCB->VTOR = QSPI_BASE;
    return 0;
}
INIT_BOARD_EXPORT(vtor_config);
