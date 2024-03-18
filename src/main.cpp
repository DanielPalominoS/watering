#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h> //Include File System Headers
#include "LittleFS.h"
#include <Adafruit_BMP280.h>

#define relay 5

unsigned int min_on_interval = 5;
unsigned int activations_per_day = 2;
unsigned int min_off_interval;
bool isValveActive = false;
Adafruit_BMP280 bmp;
String ledState;

const char *htmlfile = "/index.html";

// WiFi Connection configuration
const char *ssid = "NodeMCU";//"PALOMINO SUAREZ";
const char *password = "12345678";//"suarez8911";
IPAddress local_ip(192,168,4,2);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

const int valvePin = D3;
unsigned long previousMillis = 0;
unsigned long currentMillis;

ESP8266WebServer server(80);

unsigned int compute_off_time()
{
  min_off_interval = (24 * 60 - min_on_interval * activations_per_day) / activations_per_day;
  return min_off_interval;
}

String getTemperature()
{
  float temperature = bmp.readTemperature();
  temperature = 20;
  Serial.println(temperature);
  return String(temperature);
}
String getPressure()
{
  float Pressure = bmp.readPressure();
  Serial.println(Pressure);
  return String(Pressure);
}

void delay_min(int num_min)
{
  for (int i = 0; i < num_min; i++)
  {
    delay(60000);
  }
}

bool loadFromLittleFS(String path)
{
  String dataType = "text/plain";
  if (path.endsWith("/"))
    path += "index.htm";

  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html"))
    dataType = "text/html";
  else if (path.endsWith(".htm"))
    dataType = "text/html";
  else if (path.endsWith(".css"))
    dataType = "text/css";
  else if (path.endsWith(".js"))
    dataType = "application/javascript";
  else if (path.endsWith(".png"))
    dataType = "image/png";
  else if (path.endsWith(".gif"))
    dataType = "image/gif";
  else if (path.endsWith(".jpg"))
    dataType = "image/jpeg";
  else if (path.endsWith(".ico"))
    dataType = "image/x-icon";
  else if (path.endsWith(".xml"))
    dataType = "text/xml";
  else if (path.endsWith(".pdf"))
    dataType = "application/pdf";
  else if (path.endsWith(".zip"))
    dataType = "application/zip";
  File dataFile = LittleFS.open(path.c_str(), "r");
  if (server.hasArg("download"))
    dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size())
  {
  }

  dataFile.close();
  return true;
}
void handleWebRequests()
{
  if (loadFromLittleFS(server.uri()))
    return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}
void handleRoot()
{
  server.sendHeader("Location", "/index.html", true); // Redirect to our html web page
  server.send(302, "text/plane", "");
}
void activateValve()
{
  previousMillis = millis();
  digitalWrite(valvePin, HIGH); // LED ON
  isValveActive = true;
  server.send(200, "text/plane", "ON"); // Send web page
}

void readValve()
{
  String label = "OFF";
  if (digitalRead(valvePin) == HIGH)
  {
    label = "ON";
  }
  Serial.println(label);
  server.send(200, "text/html", label.c_str());
}
void handleActivations()
{
  String t_state = server.arg("Activations"); // Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
  Serial.println(t_state);
  activations_per_day = (unsigned int)t_state.toInt();
  server.send(200, "text/plane", "success"); // Send web page
}
void handleActiveTime()
{
  String t_state = server.arg("ActiveTime"); // Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
  Serial.println(t_state);
  min_on_interval = (unsigned int)t_state.toInt();
  server.send(200, "text/plane", "success"); // Send web page
}
void handleTemperature()
{
  server.send(200, "text/html", getTemperature().c_str());
}
void handlePressure()
{
  server.send(200, "text/html", getPressure().c_str());
}

void setup()
{
  pinMode(valvePin,OUTPUT);
  pinMode(D0,OUTPUT);
  digitalWrite(D0,LOW);
  min_off_interval = compute_off_time();
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  // Initialize File System
  if (LittleFS.begin())
  {
    Serial.println("File System Initialized");
  }
  else
  {
    Serial.println("An Error has occurred while mounting File system");
  }

  // Connect to wifi Network
  //WiFi.begin(ssid, password); // Connect to your WiFi router
  //WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);  
  delay(1000);
  // if (!bmp.begin(0x76))
  // {
  //   Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //   while (1)
  //     ;
  // }

  // Wait for connection
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(".");
  // }

  // If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  //Serial.println(WiFi.localIP()); // IP address assigned to your ESP

  // Initialize Webserver
  server.on("/", handleRoot);
  server.on("/setValve", activateValve);
  server.on("/getValve", readValve);
  server.on("/temperature", handleTemperature);
  server.on("/setActiveTime", handleActiveTime);
  server.on("/setActivations", handleActivations);
  server.on("/pressure", handlePressure);
  // Route to set GPIO to HIGH

  server.onNotFound(handleWebRequests); // Set setver all paths are not found so we can handle as per URI
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{

  server.handleClient();
  currentMillis = millis();

  if (isValveActive)
  {
    if ((currentMillis - previousMillis >= min_on_interval * 60*1000))
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      isValveActive = false;
      // set the LED with the ledState of the variable:
      digitalWrite(valvePin, LOW);
    }
  }
  else
  {
    if ((currentMillis - previousMillis >= min_off_interval *60* 1000))
    {
      previousMillis = currentMillis;
      isValveActive = true;
      // set the LED with the ledState of the variable:
      digitalWrite(valvePin, HIGH);
    }
  }
  if ((millis() >= 60 * 60 * 24 * 7 * 1000))
  {
    ESP.restart();
  }
}
