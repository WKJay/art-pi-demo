/*************************************************
Copyright (c) 2020
All rights reserved.
File name:     wifi.c
Description:   
History:
1. Version:    V1.0.0
Date:      2020-09-15
Author:    WKJay
Modify:    
*************************************************/
#include <rtthread.h>
#include <rtdevice.h>
#include <cJSON.h>
#include <wlan_mgnt.h>
#include <netdev_ipaddr.h>
#include <netdev.h>

#define DBG_TAG "wifi"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define WIFI_DEV_NAME "w0"

#define MAX_SSID_PASSWD_STR_LEN 50

struct _wifi
{
    char ssid[MAX_SSID_PASSWD_STR_LEN];
    char passwd[MAX_SSID_PASSWD_STR_LEN];
} wifi;

int wifi_connect(char *conn_str)
{
    cJSON *conn = cJSON_Parse(conn_str);
    if (conn)
    {
        cJSON *ssid = cJSON_GetObjectItem(conn, "ssid");
        cJSON *passwd = cJSON_GetObjectItem(conn, "passwd");
        if (ssid && passwd)
        {
            if (rt_strlen(ssid->valuestring) > MAX_SSID_PASSWD_STR_LEN ||
                rt_strlen(passwd->valuestring) > MAX_SSID_PASSWD_STR_LEN)
            {
                LOG_E("invalid ssid or passwd length,max %d", MAX_SSID_PASSWD_STR_LEN);
            }
            else
            {
                rt_memcpy(wifi.ssid, ssid->valuestring, rt_strlen(ssid->valuestring));
                rt_memcpy(wifi.passwd, passwd->valuestring, rt_strlen(passwd->valuestring));
                return rt_wlan_connect(wifi.ssid, wifi.passwd);
            }
        }
        else
        {
            LOG_E("cannot find ssid or password.");
        }

        cJSON_Delete(conn);
    }
    else
    {
        LOG_E("invalid wifi connection string.");
    }
    return -1;
}

int wifi_init(void)
{
    rt_memset(&wifi, 0, sizeof(wifi));
    return 0;
}

int wifi_is_ready(void)
{
    return rt_wlan_is_ready();
}

char* wifi_get_ip(void)
{
    rt_device_t netdev = rt_device_find(WIFI_DEV_NAME);
    if(netdev)
    {
        return inet_ntoa(((struct netdev *)netdev)->ip_addr);
    }
    else
    {
        return NULL;
    }
}
