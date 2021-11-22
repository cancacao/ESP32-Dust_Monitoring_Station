#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "SYS_GET_DATA"

const char *ssid = "linksys";
const char *password = "DCS@12345";

unsigned long previousMillis = 0;
const long interval = 5000;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connect_to_broker()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      client.subscribe(MQTT_TOPIC);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  char status[20];
  Serial.println("-------new message from broker-----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  Serial.write(payload, length);
  Serial.println();
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  connect_to_broker();
  Serial.println("Start transfer");
}

void loop()
{
  // put your main code here, to run repeatedly:
  client.loop();
  if (!client.connected())
  {
    connect_to_broker();
  }
  client.publish(MQTT_TOPIC, "hi broker!");
  delay(3000);
}