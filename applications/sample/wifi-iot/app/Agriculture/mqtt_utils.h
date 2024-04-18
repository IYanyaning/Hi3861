#ifndef MQTT_UTILS_H
#define MQTT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include <unistd.h>
#include "hi_wifi_api.h"
//#include "wifi_sta.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "MQTTPacket.h"
#include "transport.h"
// #include "traffic_light.h"



void get_f(float temp,float humi,  unsigned short soilhumi);
void get_state(int fanState,int waterState);
int get_mqtt_server_flag(void);

int mqtt_connect(void);

int fan_switch(void);

int change_fan(void);

int water_switch(void);

int change_water(void);

float change_humiThreshold(void);
float change_tempThreshold(void);


#endif
