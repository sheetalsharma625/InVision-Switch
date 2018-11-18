#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "InVision";
const char *password = "invisi0n";
const char *centralServer = "192.168.4.1";
const String deviceId = "00001";
bool updateRequired = false;

ESP8266WebServer server(80);
HTTPClient httpClient;
os_timer_t myTimer;

const int switchPin = 14;

void registerWithCentralServer()
{
  httpClient.begin(centralServer, 8080, "/switches");
  httpClient.addHeader("Content-Type", "application/json");
  String request = "{\"id\": \"" + deviceId + "\", \"state\": " + digitalRead(switchPin) + "}";
  int code = httpClient.POST(request);
  httpClient.end();
  if (code == 200)
  {
    Serial.println("Device Registered.");
    updateRequired = false;
  }
  else
  {
    Serial.print("Failed to Register device. ");
    Serial.println(code);
  }
}

void handleSwitchOn()
{
  digitalWrite(switchPin, HIGH);
  server.send(200, "text/plain", "Switch ON");
  updateRequired = true;
}

void handleSwitchOff()
{
  digitalWrite(switchPin, LOW);
  server.send(200, "text/plain", "Switch OFF");
  updateRequired = true;
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void timerCallback(void *pArg)
{
  updateRequired = true;
}

void setup(void)
{
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  os_timer_setfn(&myTimer, timerCallback, NULL);
  os_timer_arm(&myTimer, 300000, true);

  registerWithCentralServer();

  server.on("/switch/on", HTTP_POST, handleSwitchOn);
  server.on("/switch/off", HTTP_POST, handleSwitchOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
  if (updateRequired)
  {
    registerWithCentralServer();
    delay(500);
  }
}
