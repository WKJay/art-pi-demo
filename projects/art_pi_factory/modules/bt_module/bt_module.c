/*************************************************
Copyright (c) 2020
All rights reserved.
File name:     bluetooth.c
Description:   
History:
1. Version:    V1.0.0
Date:      2020-09-16
Author:    WKJay
Modify:    
*************************************************/
#include <rtthread.h>
#include "filesystem.h"

#define DBG_TAG "wifi"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern void bt_stack_port_main(void);

void bluetooth_thread(void *param)
{
    while(!root_filesystem_is_ready())
    {
        rt_thread_mdelay(500);
    }
    bt_stack_port_main();
}

int bluetooth_init(void)
{
    rt_thread_t tid = rt_thread_create("bt_thread", bluetooth_thread, NULL, 512, 15, 5);
    if (tid)
    {
        rt_thread_startup(tid);
        return 0;
    }
    else
    {
        LOG_E("bluetooth thread create failed");
        return -1;
    }
}