
#include "mqtt_utils.h"
// #define HOST_ADDR "192.168.43.15"
#define HOST_ADDR "broker.hivemq.com"
#define CHEN_FAN "chen_fan"
#define CHEN_WATER "chen_water"
#define CHEN_TempThreshold "chen_temp"
#define CHEN_HumiThreshold "chen_humi"
#define CHEN_MQTTSERVERADDR "chen_addr"
int mqttServerFlag = -1;
char mqttAddr[100] = {"broker.hivemq.com"};
int tostop = 0;
float temp;
float humi;
unsigned short soilhumi;

float TempThreshold = 25;
float HumiThreshold = 1;

int fanSwitch = -1;
int changeFan = -1;
int waterSwitch = -1;
int changeWater = -1;
void get_f(float temp1, float humi1, unsigned short soilhumi1)
{
  temp = temp1;
  humi = humi1;
  soilhumi = soilhumi1;
}

void get_state(int fanState, int waterState){
  fanSwitch = fanState;
  waterSwitch = waterState;
}

int get_mqtt_server_flag(void){
  return mqttServerFlag;
}

int mqtt_connect(void)
{
  
  int mysock = 0;
  while(1)
  {
  mqttServerFlag = -1;
  printf("come in while mqttServer%s\n",mqttAddr);  
  int rc = 0;
  
  unsigned char buf[200];
  int buflen = sizeof(buf);
  int msgid = 1;
  MQTTString topicString = MQTTString_initializer;
  int req_qos = 0;
  char payload[200] = {0};
  int payloadlen = strlen(payload);
  char comm_data[200];

  int len = 0;
  //char *host = HOST_ADDR; // MQTT服务器的IP地址
  //char *host = 0;;
  
  int port = 1883;

  // float temp=50;
  // float humi=60;

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  mysock = transport_open(mqttAddr, port);
  if (mysock < 0){
    printf("finish mysock\n");
    return mysock;
  }
   
  printf("Sending to hostname %s port %d\n", mqttAddr, port);
  data.clientID.cstring = "chen"; // 修改成⾃⼰名称，
  data.keepAliveInterval = 60;    // ⼼跳时间
  data.cleansession = 1;
  data.username.cstring = "chen";
  data.password.cstring = "chenyaya.a";
  len = MQTTSerialize_connect(buf, buflen, &data);
  rc = transport_sendPacketBuffer(mysock, buf, len);

  /* wait for connack */
  if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
  {
    printf("start connack\n");
    unsigned char sessionPresent, connack_rc;
    if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 ||
        connack_rc != 0)
    {
      printf("Unable to connect, return code %d\n", connack_rc);
      goto exit;
    }
  }
  else{
    printf("read failed\n");
    goto exit;
  }
    



  printf("start sub\n");
  /* loop getting msgs on subscribed topic */

  /* subscribe */
  topicString.cstring = CHEN_FAN;//订阅风扇开关

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

  topicString.cstring = CHEN_WATER;//订阅水泵开关
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

  topicString.cstring = CHEN_TempThreshold;//订阅温度阈值修改

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
  topicString.cstring = CHEN_HumiThreshold;//订阅湿度阈值修改
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


   topicString.cstring = CHEN_MQTTSERVERADDR;//订阅mqtt服务器地址修改
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

  while (!tostop&&mqttServerFlag !=0)
  {
    
    /* transport_getdata() has a built-in 1 second timeout,
 your mileage will vary */
       changeFan = -1;
        changeWater = -1;
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

      

      rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_in, &payloadlen_in, buf, buflen);
      printf("serializepublish ret:%d\n", rc);
      // char *tmp_payload_in = (char * )payload_in;
      // printf("mysub:%s recetopic:%s\n",mysub,receivedTopic.cstring);
      printf("receTopicname:%s   ,len:%d\n", receivedTopic.lenstring.data, receivedTopic.lenstring.len);

      if (strncmp(CHEN_FAN, receivedTopic.lenstring.data, 8) == 0)
      {
        printf("come in fan\n");
        if (payload_in[0] == '1')
        {
          printf("1\n");
          changeFan = fanSwitch == 0 ? -1 : 0;
          fanSwitch = 0;
        }
        else if (payload_in[0] == '0')
        {
          printf("0\n");
          changeFan = fanSwitch == -1 ? -1 : 0;
          fanSwitch = -1;
        }
        else
        {
          printf("do nothing for chen_fan\n");
        }
      }
      else if (strncmp(CHEN_WATER, receivedTopic.lenstring.data, 10) == 0)
      {
        printf("come in water\n");
        if (payload_in[0] == '1')
        {
          printf("1\n");
          changeWater = waterSwitch == 0 ? -1 : 0;
          waterSwitch = 0;
        }
        else if (payload_in[0] == '0')
        {
          printf("0\n");
          changeWater = waterSwitch == -1 ? -1 : 0;
          waterSwitch = -1;
        }
        else
        {
          printf("do nothing for chen_water\n");
        }
      }
      else if (strncmp(CHEN_TempThreshold, receivedTopic.lenstring.data, 9) == 0)
      {
        TempThreshold = atof((char*)payload_in);
        printf("TempThreshold:%2.f\n", TempThreshold);
      }
      else if (strncmp(CHEN_HumiThreshold, receivedTopic.lenstring.data, 9) == 0)
      {
        HumiThreshold = atof((char*)payload_in);
        printf("HumiThreshold:%2.f\n", HumiThreshold);
      }
      else if(strncmp(CHEN_MQTTSERVERADDR, receivedTopic.lenstring.data, 9) == 0)
      {
        printf("come in addr\n");
        strcpy(mqttAddr,(char *)payload_in);
        printf("MqttServerAddr:%s\n",mqttAddr);
        mqttServerFlag = 0;
        
      }
      printf("message arrived %d,%s\n", payloadlen_in, payload_in);
      rc = rc;
    }
    //发送检测的温湿度、土壤湿度，主题为 chen_Agr
    topicString.cstring = "chen_Agr";
    sprintf(comm_data, "temp:%.2f, humi:%.2f, soilhumi:%d", temp, humi, soilhumi);
    printf("temp:%.2f, humi:%.2f, soilhumi:%d\n", temp, humi, soilhumi);
    memcpy(payload, comm_data, 200);

    payloadlen = strlen(payload);

    len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);

    rc = transport_sendPacketBuffer(mysock, buf, len);

    printf("len:%d,rc:%d\n", len, rc);

    //发送风扇、水泵的状态以及温度、湿度阈值,mqtt服务器地址
    topicString.cstring = "chen_state";
    sprintf(comm_data, "FanState:%d, WaterState:%d, TempThreshold:%.2f, HumiThreshold:%.2f, MqttServer:%s",fanSwitch, waterSwitch,TempThreshold,HumiThreshold,mqttAddr);
    printf("FanState:%d, WaterState:%d, TempThreshold:%.2f, HumiThreshold:%.2f, MqttServer",fanSwitch, waterSwitch,TempThreshold,HumiThreshold,mqttAddr);
    memcpy(payload, comm_data, 200);

    payloadlen = strlen(payload);

    len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);

    rc = transport_sendPacketBuffer(mysock, buf, len);

    printf("len:%d,rc:%d\n", len, rc);
    
    
    sleep(3);
  }

exit:
  
  printf("cloase mqtt\n");
  transport_close(mysock);
  rc = rc;
  }
  
  return 0;

  
}

int fan_switch(void)
{
  return fanSwitch;
}
int change_fan(void)
{
  return changeFan;
}
int water_switch(void)
{
  return waterSwitch;
}
int change_water(void)
{
  return changeWater;
}
float change_humiThreshold(void)
{
  return HumiThreshold;
}
float change_tempThreshold(void){
  return TempThreshold;
}