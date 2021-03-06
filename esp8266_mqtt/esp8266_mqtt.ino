#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include "DHT.h"
#define DHTTYPE DHT11
#define dht_dpin D3
#define LEDpin D5

const char* ssid = "ChinaNet-zFQ6";
const char* password = "vdtkpnuw";
const char* mqttServer = "47.115.60.188";
const String subtopic = "pubtest";
float h;
float t;

Ticker ticker;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
const size_t capacity = JSON_OBJECT_SIZE(1) + 15;
DynamicJsonDocument doc(capacity);
DHT dht(dht_dpin, DHTTYPE); 

void setup() {
  // put your setup code here, to run once:
  //打开串口
  dht.begin();
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);     // 设置板上LED引脚为输出模式
  digitalWrite(LED_BUILTIN, HIGH);  // 启动后关闭板上LED
  pinMode(LEDpin, OUTPUT);     // 设置D3引脚为输出模式
  digitalWrite(LEDpin, HIGH);  // 启动后关闭板上LED 
  //链接WiFi
  connectWifi();

  //设置mqtt的服务器和端口号
  mqttClient.setServer(mqttServer, 1883);
  
  // 设置MQTT订阅回调函数
  mqttClient.setCallback(receiveCallback);
  
  //链接mqtt服务器
  connectMQTTServer();

  ticker.attach(3,pubMQTTmsg);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(mqttClient.connected()){
    mqttClient.loop();
//    pubmsg = pubmsgcon + (String)(value++);
//    pubMQTTmsg(pubtopic,pubmsg);
//    delay(5000);
  }else{
    connectMQTTServer();
  }
  h = dht.readHumidity();
  t = dht.readTemperature();
  delay(1000);
}

//链接mqtt服务器
void connectMQTTServer(){
  String clientId = "esp8266-" + WiFi.macAddress();
  if (mqttClient.connect(clientId.c_str())) { 
    Serial.println("MQTT Server Connected.");
    Serial.print("Server Address: ");
    Serial.println(mqttServer);
    Serial.print("ClientId:");
    Serial.println(clientId);
    subscribeTopic(subtopic);
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000);
  }   
}

//发布消息
void pubMQTTmsg(){
  String topic="subtest";
  //String topicString = "Taichi-Maker-Pub-" + WiFi.macAddress();
  char publishTopic[topic.length() + 1];  
  strcpy(publishTopic, topic.c_str());
 
  // 建立发布信息。信息内容以Hello World为起始，后面添加发布次数。
  String message="{\"temperature\":"+(String)t+",\"humidity\":"+(String)h+"}";
  char publishMsg[message.length() + 1];   
  strcpy(publishMsg, message.c_str());
  
  // 实现ESP8266向主题发布信息
  if(mqttClient.publish(publishTopic, publishMsg)){
//    Serial.print("Publish Topic:");
//    Serial.println(publishTopic);
    Serial.print("Publish message:");
    Serial.println(publishMsg);    
  } else {
    Serial.println("Message Publish Failed."); 
  }
}

// 订阅指定主题
void subscribeTopic(String topic){
  char subTopic[topic.length() + 1];  
  strcpy(subTopic, topic.c_str());

  if(mqttClient.subscribe(subTopic)){
    Serial.print("Subscrib Topic:");
    Serial.println(subTopic);
  } else {
    Serial.println("Subscribe Fail...");
  }  
}


// 收到信息后的回调函数
void receiveCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("]:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  Serial.print("Message Length(Bytes) ");
  Serial.println(length);

  deserializeJson(doc, payload);
  int LED = doc["LED_status"].as<int>();
  
  if(LED==1){
    digitalWrite(LEDpin, 1);
    Serial.println("LED OFF. command："+(String)LED);
  }else if(LED==0){
    digitalWrite(LEDpin, 0); 
    Serial.println("LED ON. command："+(String)LED);
  }
  
}

//链接WiFi
void connectWifi(){
  //设置esp8266工作模式为无线终端模式
  WiFi.mode(WIFI_STA);
  delay(500);
  
  //开始链接WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  //等待WiFi连接,成功连接后输出成功信息
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");  
  Serial.print("localIP is："); 
  Serial.println(WiFi.localIP());
}
