#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid     = "your ssid";
const char* password = "your password";

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;  // default MQTT port
const char* sub_topic = "mytopic456";

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  DynamicJsonDocument doc(128);
  deserializeJson(doc, payload);

  long value1 = doc["value1"];
  long value2 = doc["value2"];

  Serial.print("Value1: ");
  Serial.println(value1);

  Serial.print("Value2: ");
  Serial.println(value2);
}

void setup() {
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())) {
      Serial.println("connected");
      bool sub = client.subscribe(sub_topic);
      if (sub) {
        Serial.println("Subscribed");
      } else {
        Serial.println("Not subscribed");
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);      
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  

  // Do something else
}
