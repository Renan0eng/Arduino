void Sensor(void* pvParameters) {
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.begin(12), DEC); // Corrigido para passar a resolução como argumento
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0)) 
    Serial.println("Sensores nao encontrados!");

  while (true) {
    float newTempCLoca = dht.readTemperature();
    float newHumidity = dht.readHumidity();

    if (isnan(newTempCLoca) || isnan(newHumidity)) {
      Serial.println("Falha na leitura do sensor DHT!");
    } else {
      tempCLoca = newTempCLoca;
      humidity = newHumidity;
    }

    if (sensors.isConversionDone()) {
      tempC = sensors.getLatestTempC(0);
      sensors.startConvertion(); // Corrigido o nome do método
    }

    Serial.print("Umidade: ");
    Serial.print(humidity);
    Serial.print("% / Temperatura Local: ");
    Serial.print(tempCLoca);
    Serial.print("*C / Temperatura DS18B20: ");
    Serial.print(tempC);
    Serial.println("*C");

    delay(2000);
  }
}