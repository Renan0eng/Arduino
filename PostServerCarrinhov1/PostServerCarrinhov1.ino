#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "RENAN";
const char* password = "01112003";

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

void handleRoot() {
  String page = "<html lang='pt-br'>\
  <head>\
  <meta charset='UTF-8'>\
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
  <title>Joystick</title>\
  <style>\
    body, html { margin: 0; padding: 0; height: 100%; }\
    .joystick { position: relative; width: 200px; height: 200px; background-color: #222; border-radius: 50%; margin: 100px auto; display: flex; justify-content: center; align-items: center;}\
    .stick { width: 60px; height: 60px; background-color: #999; border-radius: 50%; cursor: pointer; left: 50%; top: 50%; transform: translate(-50%, -50%); }\
  </style>\
</head>\
<body>\
  <div class='joystick'>\
    <div class='stick' id='stick'></div>\
    <script>\
      var stick = document.getElementById('stick');\
      var joystick = document.querySelector('.joystick');\
      var joystickRect = joystick.getBoundingClientRect();\
      var centerX = joystickRect.left + joystickRect.width / 2;\
      var centerY = joystickRect.top + joystickRect.height / 2;\
      stick.addEventListener('mousedown', startDrag);\
      document.addEventListener('mousemove', drag);\
      document.addEventListener('mouseup', stopDrag);\
      function startDrag(e) {\
        e.preventDefault();\
        document.body.style.userSelect = 'none';\
        stick.style.transition = 'none';\
        document.addEventListener('mousemove', drag);\
        document.addEventListener('mouseup', stopDrag);\
      }\
      function drag(e) {\
        var posX = e.clientX - centerX;\
        var posY = e.clientY - centerY;\
        var distance = Math.sqrt(posX * posX + posY * posY);\
        var maxDistance = joystickRect.width / 2 - stick.offsetWidth / 2;\
        if (distance <= maxDistance) {\
          stick.style.transform = `translate(${posX}px, ${posY}px)`;\
        } else {\
          var angle = Math.atan2(posY, posX);\
          var x = Math.cos(angle) * maxDistance;\
          var y = Math.sin(angle) * maxDistance;\
          stick.style.transform = `translate(${x}px, ${y}px)`;\
        }\
        sendCommand(posX, posY);\
      }\
      function stopDrag() {\
        document.body.style.userSelect = 'auto';\
        stick.style.transition = 'transform 0.1s ease';\
        stick.style.transform = 'none';\
        document.removeEventListener('mousemove', drag);\
        document.removeEventListener('mouseup', stopDrag);\
      }\
      function sendCommand(posX, posY) {\
        var angle = Math.atan2(posY, posX);\
        var distance = Math.sqrt(posX * posX + posY * posY);\
        var maxDistance = joystickRect.width / 2 - stick.offsetWidth / 2;\
        var command = '';\
        if (distance > 20) { \
          if (angle >= -Math.PI / 4 && angle <= Math.PI / 4) {\
            command = 'right';\
          } else if (angle >= Math.PI / 4 && angle <= 3 * Math.PI / 4) {\
            command = 'down';\
          } else if (angle >= -3 * Math.PI / 4 && angle <= -Math.PI / 4) {\
            command = 'up';\
          } else {\
            command = 'left';\
          }\
        } else {\
          command = 'stop';\
        }\
        console.log('Comando enviado: ' + command);\
      }\
    </script>\
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
