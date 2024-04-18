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
 
void connect_wifi(const char * wifi_ssid, const char * wifi_password)
 {
    WifiErrorCode errCode;
    WifiDeviceConfig apConfig = {};
    int netId = -1;
    strcpy(apConfig.ssid, wifi_ssid);       //
    strcpy(apConfig.preSharedKey, wifi_password); //密码
    apConfig.securityType = WIFI_SEC_TYPE_PSK;
    errCode = EnableWifi();
    errCode = AddDeviceConfig(&apConfig, &netId);
    errCode = ConnectTo(netId);
    printf("ConnectTo(%d): %d\r\n", netId, errCode);
    usleep(1000 * 1000);
    struct netif *iface = netifapi_netif_find("wlan0");
    if (iface)
    {
        err_t ret = netifapi_dhcp_start(iface);
        printf("netifapi_dhcp_start: %d\r\n", ret);
        usleep(2000 * 1000);
        ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
        printf("netifapi_netif_common: %d\r\n", ret);
    }
 }
 #endif