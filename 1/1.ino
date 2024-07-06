#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#else
  #include <WiFi.h>
  #include <WebServer.h>
  #include <HTTPClient.h>
#endif

#include <DNSServer.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Configurações do sensor DHT
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float tempCLoca = 0;
float humidity = 0;

// Configuração do watchdog
hw_timer_t *timer = NULL;
void IRAM_ATTR resetModule() {
    ets_printf("(watchdog) reiniciar\n");
    esp_restart();
}

void setup() {
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &resetModule, true);
  timerAlarmWrite(timer, 1000000000, true);
  timerAlarmEnable(timer);

  Serial.begin(115200);
  delay(10);

  dht.begin();
  delay(2000);

  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(240);
  if (!wifiManager.autoConnect("ESP", "esp12345")) {
    Serial.println(F("Falha na conexao. Resetar e tentar novamente..."));
    delay(3000);
    ESP.restart();
    delay(5000);
  }

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println(F("Conectado na rede Wifi."));
  Serial.print(F("Endereco IP: "));
  Serial.println(WiFi.localIP());

  // xTaskCreatePinnedToCore(
  //     Sensor,
  //     "Sensor",
  //     10000,
  //     NULL,
  //     2,
  //     NULL,
  //     0);
}

void loop() {
  delay(10000);
  timerWrite(timer, 0);

  long tme = millis();

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  WiFiClient client;

  HTTPClient http;
  http.begin("http://192.168.1.253:3000/api/esp");
  http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> doc;
  doc["time"] = tme;
  doc["temperatura_local"] = tempCLoca;
  doc["humidity_local"] = humidity;
  doc["delayLoop"] = millis() - tme;
  doc["id_esp"] = "0";
  doc["type"] = "CREATE";
  String postData;
  serializeJson(doc, postData);

  int httpResponseCode = http.POST(postData);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("response: " + response);
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
  Serial.print("tempo passado dentro do loop (ms) = ");
  tme = millis() - tme;
  Serial.println(tme);
}

void Sensor(void* pvParameters) {
  while (true) {
    float newTempCLoca = dht.readTemperature();
    float newHumidity = dht.readHumidity();

    if (isnan(newTempCLoca) || isnan(newHumidity)) {
      Serial.println("Falha na leitura do sensor DHT!");
    } else {
      tempCLoca = newTempCLoca;
      humidity = newHumidity;
    }

    Serial.print("Umidade: ");
    Serial.print(humidity);
    Serial.print("% / Temperatura Local: ");
    Serial.print(tempCLoca);
    Serial.println("*C");

    delay(2000);
  }
}