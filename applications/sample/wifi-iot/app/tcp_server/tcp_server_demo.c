#include <stdio.h>
 #include <string.h>
 #include <unistd.h>
 #include "ohos_init.h"
 #include "cmsis_os2.h"
 #include "wifi_device.h"
 #include "lwip/netifapi.h"
 #include "lwip/api_shell.h"
 #include "wifi_utils.h"
 #include "tcp_server.h"
 #define     SSID        "zzzzzzzzz"
 #define     PASSWORD    "123456789"
 static void TCPServer_Task(void *arg)
 {
    (void)arg;
    connect_wifi(SSID, PASSWORD); //连接WIFI热点
    printf("Connected OK\n");
    unsigned short port = 5001;
    tcp_server(port); //启动TCP服务函数
}
 static void TCPServer_MainLoop(void)
 {
    osThreadAttr_t attr;
attr.name = "TCPServer_Task";
 attr.attr_bits = 0U;
 attr.cb_mem = NULL;
 attr.cb_size = 0U;
 attr.stack_mem = NULL;
 attr.stack_size = 10240;
 attr.priority = osPriorityNormal;
 if (osThreadNew(TCPServer_Task, NULL, &attr) == NULL)
  {
 printf("[TCPServer] Falied to create WifiConnectTask!\n");
  }
 }
 APP_FEATURE_INIT(TCPServer_MainLoop);