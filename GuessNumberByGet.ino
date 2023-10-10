#include <WiFi.h>
#include <PubSubClient.h>

//=================================================================================================
WiFiClient   espClient;
PubSubClient client(espClient);             //สร้างออปเจ็ค สำหรับเชื่อมต่อ mqtt
//=================================================================================================
const char* ssid = "WIFI";               //wifi name
const char* password = "PASSWORD";         //wifi password
//=================================================================================================
const char* mqtt_broker = "broker.hivemq.com";   //IP mqtt server
const int   mqtt_port = 1883;               //port mqtt server
//=================================================================================================

int number;

void setup_wifi() {   //ฟังก์ชั่นเชื่อมต่อwifi
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);              //เลือกโหมดรับ wifi
  WiFi.begin(ssid, password);       //เชื่อมต่อ wifi
  while (WiFi.status() != WL_CONNECTED)     //รอจนกว่าจะเชื่อมต่อwifiสำเร็จ
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int guess;
void callback(char *topic, byte *payload, unsigned int length) {  //ฟังก์ชั่นsubscribe
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
    {Serial.println((char) payload[i]-48);
    guess = payload[i]-48;
  if(number==guess)
  {
    client.publish("CE_esp","true!");
    number = random(9);
    Serial.print("number guess :");
    Serial.println(number);
    client.publish("CE_esp","guess number!");
  }
  else if(guess>number)
  {
    client.publish("CE_esp","more!");
  }
    else if(guess<number)
  {
    client.publish("CE_esp","less!");
  }
    }
  Serial.println();
  Serial.println("-----------------------");
}

void reconnect() {  //ฟังก์ชั่นเชื่อมต่อmqtt
  client.setServer(mqtt_broker, mqtt_port);   //เชื่อมต่อmqtt
  client.setCallback(callback);               //เลือกฟังก์ชั่นsubscribe
  while (!client.connected()) //รอจนกว่าจะเชื่อมต่อmqttสำเร็จ
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str()))
      Serial.println("Public emqx mqtt broker connected");
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  setup_wifi(); //เชื่อมต่อwifi
  reconnect();  //เชื่อมต่อmqtt
  client.subscribe("CE_mqtt");  //กำหนด topic ที่จะ subscribe
  number = random(9) ;
  client.publish("CE_esp","guess number!"); //กำหนด topic ที่จะ publish และ valu
  Serial.println(number);
}

void loop()
{
  client.loop();//วนลูปรอsubscribe valu จาก mqtt
}
