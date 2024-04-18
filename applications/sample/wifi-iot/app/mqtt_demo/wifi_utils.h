 #ifndef WIFI_UTILS_H
 #define WIFI_UTILS_H
 #include <stdio.h>
 #include <string.h>
 #include <unistd.h>
 #include "ohos_init.h"
 #include "cmsis_os2.h"
 #include "wifi_device.h"
 #include "lwip/netifapi.h"
 #include "lwip/api_shell.h"
 void connect_wifi(const char * wifi_ssid, const char * wifi_password);
 #endif