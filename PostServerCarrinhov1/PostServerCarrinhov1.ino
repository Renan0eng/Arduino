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
    body, html { margin: 0; padding: 0; height: 100%; overflow: hidden; }\
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
      var intervalID = null;\
      stick.addEventListener('mousedown', startDrag);\
      stick.addEventListener('touchstart', startDragTouch);\
      document.addEventListener('mouseup', stopDrag);\
      document.addEventListener('touchend', stopDragTouch);\
      function startDrag(e) {\
          e.preventDefault();\
          document.body.style.userSelect = 'none';\
          stick.style.transition = 'none';\
          document.addEventListener('mousemove', drag);\
          document.addEventListener('mouseup', stopDrag);\
          intervalID = setInterval(sendCommand, 200); \
      }\
      function startDragTouch(e) {\
          e.preventDefault();\
          document.body.style.userSelect = 'none';\
          stick.style.transition = 'none';\
          document.addEventListener('touchmove', dragTouch);\
          document.addEventListener('touchend', stopDragTouch);\
          intervalID = setInterval(sendCommand, 200);\
      }\
      function drag(e) {\
          var posX = e.clientX - centerX;\
          var posY = e.clientY - centerY;\
          moveStick(posX, posY);\
      }\
      function dragTouch(e) {\
          var touch = e.touches[0];\
          var posX = touch.clientX - centerX;\
          var posY = touch.clientY - centerY;\
          moveStick(posX, posY);\
      }\
      function moveStick(posX, posY) {\
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
      }\
      function stopDrag() {\
          document.body.style.userSelect = 'auto';\
          stick.style.transition = 'transform 0.1s ease';\
          stick.style.transform = 'none';\
          document.removeEventListener('mousemove', drag);\
          clearInterval(intervalID); \
      }\
      function stopDragTouch() {\
          document.body.style.userSelect = 'auto';\
          stick.style.transition = 'transform 0.1s ease';\
          stick.style.transform = 'none';\
          document.removeEventListener('touchmove', dragTouch);\
          clearInterval(intervalID); \
      }\
      function sendCommand() {\
          var posX = parseFloat(stick.style.transform.split('(')[1].split('px')[0]);\
          var posY = parseFloat(stick.style.transform.split(',')[1].split('px')[0]);\
          var angle = Math.atan2(posY, posX);\
          var distance = Math.sqrt(posX * posX + posY * posY);\
          var maxDistance = joystickRect.width / 2 - stick.offsetWidth / 2;\
          var command = '';\
          var power = 0;\
          if (distance > 20) {\
              console.log('distance :', distance);\
              if (angle >= -Math.PI / 4 && angle <= Math.PI / 4) {\
                  command = 'right';\
              } else if (angle >= Math.PI / 4 && angle <= 3 * Math.PI / 4) {\
                  command = 'backward';\
              } else if (angle >= -3 * Math.PI / 4 && angle <= -Math.PI / 4) {\
                  command = 'forward';\
              } else {\
                  command = 'left';\
              }\
              power = 25;\
              if(distance > 45){\
                power = 75\
                }\
              if(distance > 60){\
                power = 100\
                }\
          } else {\
              command = 'stop';\
          }\
          console.log(`Comando enviado:  /${command}/${power}`);\
          var xhttp = new XMLHttpRequest();\
          xhttp.onreadystatechange = function () {\
              if (this.readyState == 4 && this.status == 200) {\
                  console.log('Resposta do servidor: ' + xhttp.responseText);\
              }\
          };\
          xhttp.open('POST', '/move', true);\
          xhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');\
          var params = 'command=' + encodeURIComponent(command) + '&power=' + encodeURIComponent(power);\
          xhttp.send(params);\
      }\
  </script>\
  </div>\
</body>\
</html>";

  server.send(200, "text/html", page);
}

void handleDirection() {
  String command = server.arg("command");
  String power = server.arg("power");
  Serial.println("Comando: " + command + " Potência: " + power);
  if (command == "forward") {
    digitalWrite(led, 1);
    delay(100);
    digitalWrite(led, 0);
  } else if (command == "backward") {
    digitalWrite(led, 1);
    delay(100);
    digitalWrite(led, 0);
  } else if (command == "left") {
    digitalWrite(led, 1);
    delay(100);
    digitalWrite(led, 0);
  } else if (command == "right") {
    digitalWrite(led, 1);
    delay(100);
    digitalWrite(led, 0);
  } else if (command == "stop") {
    digitalWrite(led, 1);
    delay(100);
    digitalWrite(led, 0);
  }
  server.send(200, "text/plain", "Comando: " + command + " Potência: " + power);
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
  server.on("/move", HTTP_POST, handleDirection);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
