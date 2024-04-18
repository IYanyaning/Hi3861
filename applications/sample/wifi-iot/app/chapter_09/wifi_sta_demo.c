#include "lwip/netifapi.h"
#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"

#include "lwip/api_shell.h"
#include "wifi_device.h"
#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#define SSID "IYanyan"
#define PASSWD "0000000oo"

static void WifiStaTask(void *arg)
{
    (void)arg;
    WifiErrorCode errCode;
    WifiDeviceConfig apConfig = {};
    printf("this sta\n");
    int netId = -1;
    // 设置将要连接的AP属性（wifi名称、密码、加密⽅式），
    strcpy(apConfig.ssid, SSID);                  //
    strcpy(apConfig.preSharedKey, PASSWD);        // 密码
    apConfig.securityType = WIFI_SEC_TYPE_PSK;    // 加密类型
    
    errCode = EnableWifi();                       // 启⽤STA模式
    if(errCode != WIFI_SUCCESS){
        printf("Enable wifi failed, err = %d\n",errCode);
        return ;
    }
    errCode = AddDeviceConfig(&apConfig, &netId); // 配置热点信息,⽣成netId
    printf("this sta\n");
    printf("AddDeviceConfig: %d\r\n", errCode);
    errCode = ConnectTo(netId); // 接到指定的热点
    printf("ConnectTo(%d): %d\r\n", netId, errCode);
    usleep(3000 * 1000); // 等待连接

    // 联⽹业务开始
    // 获取⽹络接⼝⽤于IP操作
    struct netif *iface = netifapi_netif_find("wlan0");
    if (iface)
    {
        // 启动DHCP客户端，获取IP地址
        err_t ret = netifapi_dhcp_start(iface);
        printf("netifapi_dhcp_start: %d\r\n", ret);
        usleep(2000 * 1000);
        // netifapi_netif_common ⽤于以线程安全的⽅式调⽤所有与netif相关的API
        // dhcp_clients_info_show为shell API，展示dhcp客户端信息
        ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
        printf("netifapi_netif_common: %d\r\n", ret);
    }
    usleep(5000 * 1000);
    printf("sta is disconnect\n");
    Disconnect();
    RemoveDevice(netId);
    // 断开Wifi连接
    // 移除Wifi热点的配置
    errCode = DisableWifi(); // 禁⽤STA模式
    printf("DisableWifi: %d\r\n", errCode);
}
static void WifiStaDemo(void)
{
    osThreadAttr_t attr;
    attr.name = "WifiStaTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;
    if (osThreadNew(WifiStaTask, NULL, &attr) == NULL)
    {
        printf("[WifiStaTask] Falied to create WifiConnectTask!\n");
    }
}
APP_FEATURE_INIT(WifiStaDemo);
/*#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/ip4_addr.h"
#include "lwip/api_shell.h"

#include "cmsis_os2.h"
#include "hos_types.h"
#include "wifi_device.h"
#include "wifiiot_errno.h"
#include "ohos_init.h"

#define DEF_TIMEOUT 15

static void WiFiInit(void);
static int WaitConnectResult(void);
static void OnWifiConnectionChangedHandler(int state, WifiLinkedInfo *info);
static void OnHotspotStaJoinHandler(StationInfo *info);
static void OnHotspotStateChangedHandler(int state);
static void OnHotspotStaLeaveHandler(StationInfo *info);

static int g_ConnectSuccess = 0;
WifiEvent g_wifiEventHandler = {0};
WifiErrorCode error;

#define SELECT_WLAN_PORT "wlan0"

#define SELECT_WIFI_SSID "IYanyan"
#define SELECT_WIFI_PASSWORD "Rem01072"
#define SELECT_WIFI_SECURITYTYPE WIFI_SEC_TYPE_PSK

static BOOL WifiSTATask(void)
{
    //WifiScanInfo *info = NULL;
    //unsigned int size = WIFI_SCAN_HOTSPOT_LIMIT;
    static struct netif *g_lwip_netif = NULL;
    WifiDeviceConfig select_ap_config = {0};

    osDelay(200);
    printf("<--System Init-->\r\n");

    //初始化WIFI
    WiFiInit();
    error = EnableWifi();

    //使能WIFI
    if (error != 0)
    {
        printf("EnableWifi failed, error = %d\n", error);
        return -1;
    }

    //判断WIFI是否激活
    if (IsWifiActive() != 0)
    {
        printf("Wifi station is not actived.\n");
        return -1;
    }
    
    int result;

    //拷贝要连接的热点信息
    strcpy(select_ap_config.ssid, SELECT_WIFI_SSID);
    strcpy(select_ap_config.preSharedKey, SELECT_WIFI_PASSWORD);
    select_ap_config.securityType = SELECT_WIFI_SECURITYTYPE;

    if (AddDeviceConfig(&select_ap_config, &result) == WIFI_SUCCESS)
    {
        if (ConnectTo(result) == WIFI_SUCCESS && WaitConnectResult() == 1)
        {
            printf("WiFi connect succeed!\r\n");
            g_lwip_netif = netifapi_netif_find(SELECT_WLAN_PORT);
        }
    }

    //启动DHCP
    if (g_lwip_netif)
    {
        dhcp_start(g_lwip_netif);
        printf("begain to dhcp");
    }


    //等待DHCP
    for(;;)
    {
        if(dhcp_is_bound(g_lwip_netif) == ERR_OK)
        {
            printf("<-- DHCP state:OK -->\r\n");

            //打印获取到的IP信息
            netifapi_netif_common(g_lwip_netif, dhcp_clients_info_show, NULL);
            break;
        }

        printf("<-- DHCP state:Inprogress -->\r\n");
        osDelay(100);
    }

    //执行其他操作
    for(;;)
    {
        osDelay(100);
    }

}

static void WiFiInit(void)
{
    printf("<--Wifi Init-->\r\n");
    g_wifiEventHandler.OnWifiConnectionChanged = OnWifiConnectionChangedHandler;
    g_wifiEventHandler.OnHotspotStaJoin = OnHotspotStaJoinHandler;
    g_wifiEventHandler.OnHotspotStaLeave = OnHotspotStaLeaveHandler;
    g_wifiEventHandler.OnHotspotStateChanged = OnHotspotStateChangedHandler;
    error = RegisterWifiEvent(&g_wifiEventHandler);
    if (error != WIFI_SUCCESS)
    {
        printf("register wifi event fail!\r\n");
    }
    else
    {
        printf("register wifi event succeed!\r\n");
    }
}

static void OnWifiConnectionChangedHandler(int state, WifiLinkedInfo *info)
{
    (void)info;

    if (state > 0)
    {
        g_ConnectSuccess = 1;
        printf("callback function for wifi connect\r\n");
    }
    else
    {
        printf("connect error,please check password\r\n");
    }
    return;
}

static void OnHotspotStaJoinHandler(StationInfo *info)
{
    (void)info;
    printf("STA join AP\n");
    return;
}

static void OnHotspotStaLeaveHandler(StationInfo *info)
{
    (void)info;
    printf("HotspotStaLeave:info is null.\n");
    return;
}

static void OnHotspotStateChangedHandler(int state)
{
    printf("HotspotStateChanged:state is %d.\n", state);
    return;
}

static int WaitConnectResult(void)
{
    int ConnectTimeout = DEF_TIMEOUT;
    while (ConnectTimeout > 0)
    {
        sleep(1);
        ConnectTimeout--;
        if (g_ConnectSuccess == 1)
        {
            printf("WaitConnectResult:wait success[%d]s\n", (DEF_TIMEOUT - ConnectTimeout));
            break;
        }
    }
    if (ConnectTimeout <= 0)
    {
        printf("WaitConnectResult:timeout!\n");
        return 0;
    }

    return 1;
}

static void WifiClientSTA(void)
{
    osThreadAttr_t attr;

    attr.name = "WifiSTATask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = 24;

    if (osThreadNew((osThreadFunc_t)WifiSTATask, NULL, &attr) == NULL)
    {
        printf("Falied to create WifiSTATask!\n");
    }
}

APP_FEATURE_INIT(WifiClientSTA);*/

