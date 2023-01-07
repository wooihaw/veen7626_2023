#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// setting PWM properties
#define LED1 21

#define MSG_BUFFER_SIZE  (128)
char msg[MSG_BUFFER_SIZE];

unsigned long lastMsg = 0;

// Network settings
const char* ssid = "your ssid";
const char* password = "your password";
const char* mqtt_server = "demo.thingsboard.io";
const unsigned int mqtt_port = 1883;
const char* sub_topic = "v1/devices/me/rpc/request/+";
const char* attr_topic = "v1/devices/me/attributes";
const char* access_token = "your device access token";


WiFiClient espClient;
PubSubClient client(espClient);

unsigned int counter = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  DynamicJsonDocument doc(128);
  deserializeJson(doc, payload);

  const char* method = doc["method"];

  String responseTopic = String(topic);
  responseTopic.replace("request", "response");
  if (strcmp(method, "getLEDStatus")==0) {
      int LED1Status = digitalRead(LED1);
      client.publish(responseTopic.c_str(), String(LED1Status).c_str());
      if (LED1Status == 0) {
          client.publish(attr_topic, "{value1: 0}");
        } else {
          client.publish(attr_topic, "{value1: 1}");
      }
  } else if (strcmp(method, "setLEDStatus")==0) {
      int LED1Status = 1 - digitalRead(LED1);
      client.publish(responseTopic.c_str(), String(LED1Status).c_str());
      int value = doc["params"];
      Serial.println("params =");
      Serial.println(value);          
      if (value==0) {
        digitalWrite(LED1, LOW);
        client.publish(attr_topic, "{value1: 0}");
      } else {
        digitalWrite(LED1, HIGH);
        client.publish(attr_topic, "{value1: 1}");
      }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), access_token, "")) {
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

void setup() {
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);

  Serial.begin(115200);

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

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Do something else

}
