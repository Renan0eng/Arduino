#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "RENAN";
const char* password = "01112003";

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

void handleRoot() {
  String page = "<html lang='en'>\
    <head>\
      <meta charset='UTF-8'>\
      <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
      <title>Joystick</title>\
      <style>\
      body, html { margin: 0; padding: 0; height: 100%; }\
      .joystick { position: relative; width: 200px; height: 200px; background-color: #333; border-radius: 50%; margin: 100px auto; }\
      .base { position: absolute; width: 130px; height: 130px; left: 50%; top: 50%; transform: translate(-50%, -50%)rotate(45deg); display: flex; flex-wrap: wrap; justify-content: space-between; align-content: space-between; }\
      .button { width: 60px; height: 60px; border-radius: 50%; background-color: #999; border: none; cursor: pointer; transform: rotate(-45deg); font-size: 16px; }\
      .button-up { grid-area: 1 / 2; }\
      .button-down { grid-area: 3 / 2; }\
      .button-left { grid-area: 2 / 1; }\
      .button-right { grid-area: 2 / 3; }\
      .joystick:before { content: ''; position: absolute; width: 100%; height: 100%; background: radial-gradient(circle at 50% 50%, rgba(0, 0, 0, 0.5), transparent); border-radius: 50%; }\
  </style>\
</head>\
<body>\
  <div class='joystick'>\
    <div class='stick'></div>\
    <div class='base'>\
      <button class='button button-up' onclick=\"moveForward()\">Up</button>\
      <button class='button button-down' onclick=\"moveBackward()\">Down</button>\
      <button class='button button-left' onclick=\"turnLeft()\">Left</button>\
      <button class='button button-right' onclick=\"turnRight()\">Right</button>\
      <script>\
        function turnLeft() {\
          sendCommand('left');\
        }\
        function turnRight() {\
          sendCommand('right');\
        }\
        function moveForward() {\
          sendCommand('forward');\
        }\
        function moveBackward() {\
          sendCommand('backward');\
        }\
        function sendCommand(command) {\
          var xhttp = new XMLHttpRequest();\
          xhttp.onreadystatechange = function() {\
            if (this.readyState == 4 && this.status == 200) {\
              console.log('Comando enviado: ' + command);\
            }\
          };\
          xhttp.open('GET', '/' + command, true);\
          xhttp.send();\
        }\
      </script>\
    </div>\
  </div>\
</body>\
</html>";

  server.send(200, "text/html", page);
}

void handleDirection() {
  String direction = server.uri();
  // Implemente aqui o código para controlar o carrinho com base na direção recebida
  // Exemplo: se direction for "/left", vire o carrinho para a esquerda
  //          se direction for "/right", vire o carrinho para a direita
  //          e assim por diante
  Serial.print("direction: ");
  Serial.println(direction);
  server.send(200, "text/plain", "Comando recebido: " + direction);
}

void handleNotFound() {
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
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/left", handleDirection);
  server.on("/right", handleDirection);
  server.on("/forward", handleDirection);
  server.on("/backward", handleDirection);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
