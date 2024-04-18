

#include <stdio.h>
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
 #define HOST_ADDR "192.168.75.128"
 int toStop = 0;
 int mqtt_connect(void)
 {
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  int rc = 0;
  int mysock = 0;
  unsigned char buf[200];
  int buflen = sizeof(buf);
  int msgid = 1;
  MQTTString topicString = MQTTString_initializer;
  int req_qos = 0;
  char payload[200];
  int payloadlen = strlen(payload);
  int len = 0;
  char *host = HOST_ADDR;  //MQTT服务器的IP地址 
  int port = 1883;
  mysock = transport_open(host, port);
  if (mysock < 0)
    return mysock;
  printf("Sending to hostname %s port %d\n", host, port);
  data.clientID.cstring = "chen";  //修改成⾃⼰名称，
  data.keepAliveInterval = 20;  //⼼跳时间
  data.cleansession = 1;
  data.username.cstring = "chen";
  data.password.cstring = "1024";
  len = MQTTSerialize_connect(buf, buflen, &data);
  rc = transport_sendPacketBuffer(mysock, buf, len);
  /* wait for connack */
  if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
  {
    unsigned char sessionPresent, connack_rc;
    if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || 
connack_rc != 0)
    {
      printf("Unable to connect, return code %d\n", connack_rc);
      goto exit;
    }
  }
  else
    goto exit;
  /* subscribe */
  topicString.cstring = "subtopic";
  len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
  rc = transport_sendPacketBuffer(mysock, buf, len);
  if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) /* wait for suback */
  {
    unsigned short submsgid;
    int subcount;
    int granted_qos;
    rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
    if (granted_qos != 0)
    {
      printf("granted qos != 0, %d\n", granted_qos);
       goto exit;
    }
  }
  else
    goto exit;
  /* loop getting msgs on subscribed topic */
  
  while (!toStop)
  {
    /* transport_getdata() has a built-in 1 second timeout,
    your mileage will vary */
    if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
    {
      unsigned char dup;
      int qos;
      unsigned char retained;
      unsigned short msgid;
      int payloadlen_in;
      unsigned char *payload_in;
      int rc;
      MQTTString receivedTopic;
      rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
                     &payload_in, &payloadlen_in, buf, buflen);
      printf("message arrived %d,%s\n", payloadlen_in, payload_in);
      rc = rc;
      
    }
    topicString.cstring = "publish";   //设置主题
    len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char 
*)payload, payloadlen);
    rc = transport_sendPacketBuffer(mysock, buf, len);
    usleep(100000);
  }
  printf("disconnecting\n");
  len = MQTTSerialize_disconnect(buf, buflen);
  rc = transport_sendPacketBuffer(mysock, buf, len);
 exit:
  transport_close(mysock);
  rc = rc;
  return 0;
 }
