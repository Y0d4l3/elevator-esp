#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_LIS3DH.h>
#include <secrets.h>

Adafruit_LIS3DH lis3dh = Adafruit_LIS3DH();

void setup()
{
  Serial.begin(115200);

  delay(4000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  if (!lis3dh.begin(0x18))
  {
    Serial.println("Couldn't find LIS3DH sensor");
    while (1)
      delay(1);
  }
  Serial.println("LIS3DH found");
}

void loop()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    sensors_event_t event;
    lis3dh.getEvent(&event);


    StaticJsonDocument<48> doc;
    doc["xValue"] = event.acceleration.x;
    doc["yValue"] = event.acceleration.y;
    doc["zValue"] = event.acceleration.z;
    String output;
    serializeJson(doc, output);

    HTTPClient http;

    http.begin(API_URI);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(output);
    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else
    {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else
  {
    Serial.println("Error in WiFi connection");
  }
  delay(10000);
}