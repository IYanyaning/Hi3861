#include <stdio.h>
 #include <string.h>
 #include <unistd.h>
 #include "ohos_init.h"
 #include "cmsis_os2.h"
 #include "wifi_device.h"
 #include "lwip/netifapi.h"
 #include "lwip/api_shell.h"
 #include "wifi_utils.h"
 #include "tcp_client.h"
 #define     SSID        "zzzzzzzzz"
 #define     PASSWORD    "123456789"
 #define     IP_ADDR     "192.168.75.128"
 static void TCPClient_Task(void *arg)
 {
  (void)arg;
    connect_wifi(SSID, PASSWORD); //连接WIFI热点
    printf("connect OK\n");
    // tcp客服端连接
    unsigned short port = 5001;
    connect_tcp_server(IP_ADDR, port);
 }
 static void TCPClient_MainLoop(void)
 {
    osThreadAttr_t attr;
    attr.name = "TCPClient_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;
    if (osThreadNew(TCPClient_Task, NULL, &attr) == NULL)
     {
        printf("[NetWorkDemo] Falied to create WifiConnectTask!\n");
     }
    }
 APP_FEATURE_INIT(TCPClient_MainLoop);