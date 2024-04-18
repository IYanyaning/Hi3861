#ifndef MQTT_UTILS_H
#define MQTT_UTILS_H

  #include <stdio.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
 #include "ohos_init.h"
 #include "cmsis_os2.h"

 #include "hi_wifi_api.h"
 //#include "wifi_sta.h"
 #include "lwip/ip_addr.h"
 #include "lwip/netifapi.h"
 #include "lwip/sockets.h"
 #include "MQTTPacket.h"
 #include "transport.h"
 // #include "traffic_light.h
 #define HOST_ADDR "192.168.43.15"
 //#define HOST_ADDR "broker.hivemq.com"

 //static char *fifoFile = "/tmp/myfifo";
 //unsigned char fifoBuffer[100];
 //int fd;
 int mqtt_connect(void);
 #endif
