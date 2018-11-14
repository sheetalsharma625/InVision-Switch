#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


const char* ssid = "JAGPREET";
const char* password = "8373905943";

ESP8266WebServer server(80);

const int switchPin = 14;

void handleSwitchOn() {
  digitalWrite(switchPin, HIGH);
  server.send(200, "text/plain", "Switch ON");
}

void handleSwitchOff() {
  digitalWrite(switchPin, LOW);
  server.send(200, "text/plain", "Switch OFF");
}

void handleNotFound() {
  digitalWrite(switchPin, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  server.on("/switch/on", HTTP_POST , handleSwitchOn);
  server.on("/switch/off", HTTP_POST , handleSwitchOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
