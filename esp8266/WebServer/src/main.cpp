#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "xxxxxx";
const char* password = "xxxxxx";

// WebServer Instantiation
ESP8266WebServer server(80);

//Build Basic Web Page using HTML
//These functions are showing LED instance when authentification is success
void LED_instance() 
{
	char temp[400];

	snprintf(temp, 400,
		"<html>\
  <head>\
  </head>\
    <body>\
    <h1>TURNING ON BUILT IN LED</h1>\
    <a href =\"/login/1\"> <button> Turn ON</button> </a>\
    <a href =\"/login/0\"> <button> Turn OFF</button> </a>\
    <a href =\"/login/2\"> <button> Flashing LED</button> </a>\
  </body>\
</html>");
	server.send(200, "text/html", temp);
//	server.send(200, "text/plain", "I'm an awasome webServer!!!");

}

void LED_ON() {
	server.send(200, "text/plain", "LED is OFF");
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
}

void LED_OFF() {
	server.send(200, "text/plain", "LED is ON");
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
}

void Flash() {

	server.send(200, "text/plain", "LED is now flashing");

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(500);
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);
}


  //Check if header is present and correct
bool is_authentified(){
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//login page, also called for disconnect
void Login(){
  Serial.begin(9600);
  String msg;
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    Serial.println("Disconnection");
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.sendHeader("Set-Cookie","ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ){
      server.sendHeader("Location","/login/");
      server.sendHeader("Cache-Control","no-cache");
      server.sendHeader("Set-Cookie","ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
  msg = "Credenciales erroneas, por favor intenta de nuevo.";
  Serial.println("Log in Failed");
  }
  String content = "<html><body><H2>Servidor web para encender el LED del ESP8266</H2><br>";
  content += "<html><body><form action='/login' method='POST'>Para iniciar sesion, usa las credenciales: admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Enviar'></form>" + msg + "<br>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot(){
  Serial.begin(9600);
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }
  String content = "<html><body><H2>Hola, te has conectado al servidor web ESP8266</H2><br>";
    content += "Ahora debes loguearte haciendo click <a href=\"/login?DISCONNECT=YES\">aqui</a></body></html>";
  server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound(){
  Serial.begin(9600);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
//Serial Comunnication and Wifi Connection kick off
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.printf("\n\nConnecting to Network: %s\n", WiFi.SSID().c_str());
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Start Web Server and notify via serial comm.
  server.begin();
  Serial.println("HTTP server started");
  Serial.printf("\n\nUse this URL to connect: http://%s/\n", WiFi.localIP().toString().c_str());

  server.on("", handleRoot);
  server.on("/", handleRoot);
  server.on("/login", Login);
  server.on("/login/", LED_instance);
	server.on("/login/0", LED_ON);
	server.on("/login/1", LED_OFF);
  server.on("/login/2", Flash);

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
}

void loop(void){
  //Handle client requests to web Server using .handleClient() method from ESP8266WebServer class
  server.handleClient();
}