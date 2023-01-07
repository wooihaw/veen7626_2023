#include <WiFi.h>
#include <ArduinoHttpClient.h>

const char* ssid     = "your ssid";
const char* password = "your password";

const char* host = "dweet.io";
const char* thingname = "myiot123";

WiFiClient client;
HttpClient httpClient = HttpClient(client, host, 80);

void setup()
{
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
}

void loop()
{
    delay(5000);

    Serial.print("connecting to ");
    Serial.println(host);

    String url = "/dweet/for/";
    url += thingname;
    url += "?temperature=";
    url += random(25, 36);

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // Send a HTTP GET request to the server
    httpClient.get(url);

      // Read the response body
    String body = httpClient.responseBody();
    Serial.print("Body: ");
    Serial.println(body);
}