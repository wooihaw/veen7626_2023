#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid     = "your ssid";
const char* password = "your password";

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;  // default MQTT port
const char* pub_topic = "mytopic123";

#define MSG_BUFFER_SIZE 128
char msg[MSG_BUFFER_SIZE];

long counter1 = 0;
long counter2 = 10000;

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

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);      
    }
}

void loop() {
  DynamicJsonDocument doc(128);
  doc["counter1"] = counter1++;
  doc["counter2"] = counter2--;
  serializeJsonPretty(doc, Serial);
  serializeJson(doc, msg);
  client.publish(pub_topic, msg);
  delay(5000);
}
