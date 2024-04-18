#include "wifi_utils.h"
 void connect_wifi(const char * wifi_ssid, const char * wifi_password)
 {
 WifiErrorCode errCode;
 WifiDeviceConfig apConfig = {};
 int netId = -1;
 // setup your AP params
 strcpy(apConfig.ssid, wifi_ssid);     
//
 strcpy(apConfig.preSharedKey, wifi_password); //密码
apConfig.securityType = WIFI_SEC_TYPE_PSK;   //加密⽅式
errCode = EnableWifi();
 errCode = AddDeviceConfig(&apConfig, &netId);
 errCode = ConnectTo(netId);
 printf("ConnectTo(%d): %d\r\n", netId, errCode);
 usleep(1000 * 1000);
 // 联⽹业务开始
// 这⾥是⽹络业务代码...
 struct netif *iface = netifapi_netif_find("wlan0");
 if (iface)
  {
 err_t ret = netifapi_dhcp_start(iface); //返回IP地址
printf("netifapi_dhcp_start: %d\r\n", ret);
 // usleep(2000 * 1000);
 sleep(5);
  ; 
// wait DHCP server give me IP
 ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
 printf("netifapi_netif_common: %d\r\n", ret);
  }
 }