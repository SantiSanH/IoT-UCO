#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <WString.h>

// Variable and constant declaration
String JSON_time = "";
HTTPClient http;
WiFiClient eclient;
WiFiClient espClient;
PubSubClient client(espClient);

bool newMessage = false;
const char *today_is = "";
char publish[500];
char char_url[500];
int day = 0;

const char *ssid = "xxxxx";
const char *password = "xxxxx";
const char *mqttServer = "xxxxx";
const int mqttPort = 1883;
const char *tInput = "input";
const char *tAlive = "alive";
const char *StatusRequest = "StatusRequest";
const char *JSON_status = "JsonStatus";
const char *tOutput = "Output";

// Any function initialization
String HTTPRequest(String url);
String JSONresponse(String JSON_time);

enum month
{
  January,
  February,
  March,
  April,
  May,
  June,
  July,
  August,
  September,
  October,
  November,
  December
};

//HTTP Server URL
String link = "http://worldtimeapi.org/api/timezone/";

/********* Setup wifi ***************************
   setup wifi connect to wifi with the constants
   defined up
   while does not connect print a "."
   if connect then print the local ip
************************************************/
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/********* MQTT Callback ***************************
   here is defined the logic to execute after
   a messages arrives in the desired
   topic, for this example the variable:
   TopicSub
************************************************/
void callback(char *topic, byte *payload, unsigned int length)
{
  //Notify about message arrived
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  //Print the message received
  Serial.print("Message:");

  String message = "";
  String URLtopost = link;
  for (unsigned int i = 0; i < length; i++)
  {
    //Saves message received converting it to String
    message += (char)payload[i];
    //Adds URL extension to get response from server
    URLtopost += (char)payload[i];
    newMessage = true;
  }

  //Prints with visualization purposes
  Serial.println();
  Serial.print(message);
  Serial.println();
  Serial.println("-----------------------");
  Serial.print(URLtopost);
  Serial.println();
  Serial.println("-----------------------");
  HTTPRequest(URLtopost);
  Serial.println();
  Serial.println("-----------------------");
}

String day_name(int day)
{
  switch (day)
  {
  case 1:
    return "Lunes,";
    break;
  case 2:
    return "Martes,";
    break;
  case 3:
    return "Miércoles,";
    break;
  case 4:
    return "Jueves,";
    break;
  case 5:
    return "Viernes,";
    break;
  case 6:
    return "Sábado,";
    break;
  case 7:
    return "Domingo,";
    break;
  }
  return "";
}

String month_name(uint8 month)
{
  switch (month)
  {
  case January:
    return "Enero";
    break;
  case February:
    return "Febrero";
    break;
  case March:
    return "Marzo";
    break;
  case April:
    return "Abril";
    break;
  case May:
    return "Mayo";
    break;
  case June:
    return "Junio";
    break;
  case July:
    return "Julio";
    break;
  case August:
    return "Agosto";
    break;
  case September:
    return "Septiembre";
    break;
  case October:
    return "Octubre";
    break;
  case November:
    return "Noviembre";
    break;
  case December:
    return "Diciembre";
    break;
  }
  return "";
}

// This function compose a date with the String getted from API server
String compose_date(char year[4], uint8 month, char day_of_month[2], char day_of_week, char hour[5])
{
  String date_formated = "";
  date_formated += day_name(day_of_week);
  date_formated += " ";
  date_formated += day_of_month[0];
  date_formated += day_of_month[1];
  date_formated += " de ";
  date_formated += month_name(month);
  date_formated += " de ";
  date_formated += year[0];
  date_formated += year[1];
  date_formated += year[2];
  date_formated += year[3];
  date_formated += " -- ";
  date_formated += hour[0];
  date_formated += hour[1];
  date_formated += hour[2];
  date_formated += hour[3];
  date_formated += hour[4];
  return date_formated;
}

String HTTPRequest(String url)
{

  if (http.begin(eclient, url)) //Iniciar conexión
  {
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET(); // Realizar petición

    if (httpCode > 0)
    {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        JSON_time = http.getString(); // Obtener respuesta
        Serial.println(JSON_time);    // Mostrar respuesta por serial
        client.publish(StatusRequest, "OK");
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  else
  {
    Serial.printf("[HTTP} Unable to connect\n");
  }
  delay(1000);
  JSONresponse(JSON_time);
  //JSONresponse(publish);
  return JSON_time;
}

//JSON response about HTTP request
String JSONresponse(String JSON_time)
{
  // Convert String to Json object
  char json[500];
  JSON_time.toCharArray(json, 500);

  DynamicJsonDocument doc(500);
  deserializeJson(doc, JSON_time);

  // Get year, month, day and hour
  const char *today_is = doc["datetime"];
  day = doc["day_of_week"];

  char year[4] = {today_is[0], today_is[1], today_is[2], today_is[3]};
  char day_of_month[2] = {today_is[8], today_is[9]};
  char hour[5] = {today_is[11], today_is[12], today_is[13], today_is[14], today_is[15]};
  const uint8 month = (10 * (today_is[5] - 48) + (today_is[6] - 48)) - 1; //Convert ascii to integer

  // Generate date in right format;
  String date_formated = compose_date(year, month, day_of_month, day, hour);
  client.publish(JSON_status, "Se ha procesado el JSON");

  // Convert String to char object

  date_formated.toCharArray(publish, 500);
  Serial.print(publish);
  client.publish(tOutput, publish);
  return publish;
}

void setup()
{

  //Start Serial Communication
  Serial.begin(115200);

  //Connect to WiFi
  setup_wifi();

  //Connect to MQTT Broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  //MQTT Connection Validation
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client"))
    {
      client.publish(tAlive, "Cliente contectado al Broker");
      Serial.println("connected");
    }
    else
    {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.subscribe(tInput);
}

void loop()
{
  //MQTT client loop
  client.loop();
}
