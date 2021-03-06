#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "sys_get_data"

const char *ssid = "Can";
const char *password = "cancacao112223";

unsigned long previousMillis = 0;
const long interval = 5000;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const int pinTemp = 32;
const int pinPress = 33;
const int pinFlow = 34;
const int pintPm = 35;

int temp = 0;
int press = 0;
int flow = 0;
int pm = 0;

/* ADC 0 -> 3.3V ~ 0 -> 4095
    analogRead(GPIO)



*/
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
      Serial.println("MQTT connected");
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

float getValueSensor(int valueRead, float minRange, float maxRange)
{
  return ((maxRange - minRange) / 4095.0) * (static_cast<float>(valueRead)) + minRange;
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  //client.setCallback(callback);
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
  temp = analogRead(pinTemp);
  flow = analogRead(pinFlow);
  press = analogRead(pinPress);
  pm = analogRead(pintPm);
  DynamicJsonDocument sensorValue(1024);
  // sensorValue["temp"] = String(getValueSensor((rand() % 50) + 1500, 0, 300));
  // sensorValue["pressure"] = String(getValueSensor((rand() % 50) + 2500, 0, 2000));
  // sensorValue["flow"] = String(getValueSensor((rand() % 50) + 4200, 0, 50000));
  // sensorValue["pm"] = String(getValueSensor((rand() % 20) + 700, 0, 600));
  sensorValue["temp"] = String(getValueSensor(temp, 0, 300));
  sensorValue["pressure"] = String(getValueSensor(press, 0, 2000));
  sensorValue["flow"] = String(getValueSensor(flow, 0, 50000));
  sensorValue["pm"] = String(getValueSensor(pm, 0, 600));
  char out[128];
  int tmp = serializeJson(sensorValue, out);
  client.publish(MQTT_TOPIC, out);
  Serial.println("Published!");
  delay(15000);
}