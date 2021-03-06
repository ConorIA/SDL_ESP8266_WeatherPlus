//
// WeatherUnderground Interface
//
// SwitchDoc Labs, Spetember 2016
//
//

int sendWeatherUndergroundData() {
  Serial.print("WeatherUnderground: Begin");
  WiFiClient client;
  const char* host = "weatherstation.wunderground.com";
  const int httpPort = 80;
  unitSystem usa_units = USA;

  if (!client.connect(host, httpPort)) {
    Serial.println("WU: connection failed");
    return 1;
  }

  String myURL;
  // Sample "/weatherstation/updateweatherstation.php?ID=KWALIBER14&PASSWORD=20mqkah7&dateutc=now&winddir=180&windspeedmph=3.45&humidity=14&tempf=91.04&rainin=0.00&baromin=27.83&indoortempf=32.00&indoorhumidity0.00=&software=OurWeather";
  // Setup base URL.
  myURL = "/weatherstation/updateweatherstation.php?";
  myURL += "ID=" + WeatherUnderground_StationID;
  myURL += "&PASSWORD=" + WeatherUnderground_StationKey;
  myURL += "&dateutc=now";

  // Add weather station variables.
  myURL += "&winddir=" + String(currentWindDirection);
  myURL += "&windspeedmph=" + formatWindspeedString(currentWindSpeed, usa_units, 2);
  myURL += "&windgustmph=" + formatWindspeedString(currentWindGust, usa_units, 2);
  myURL += "&humidity=" + formatHumidityString(AM2315_Humidity, 2);
  myURL += "&tempf=" + formatTemperatureString(AM2315_Temperature, usa_units, 2);
  myURL += "&dewptf=" + formatTemperatureString(AM2315_Dewpoint, usa_units, 2);
  myURL += "&rainin=" + formatRainfallString(rain60Minutes, usa_units, 2);
  myURL += "&dailyrainin=" + formatRainfallString(rainCalendarDay, usa_units, 2);
  myURL += "&baromin=" + formatPressureString(BMP180_Pressure, usa_units, 2);
  myURL += "&indoortempf=" + formatTemperatureString(BMP180_Temperature, usa_units, 2);
//  myURL += "&indoorhumidity%0.2f=" % HTUhumidity
  myURL += "&software=OurWeather";

  Serial.print("WU URL: ");
  Serial.println(myURL);

  // Send the request to the Weather Underground.
  client.print(
      String("GET ") + myURL + " HTTP/1.1\r\n" + "Host: " + host + "\r\n"
          + "Connection: close\r\n\r\n");
  delay(1500);

  String WUResult;

  // Read all the lines of the reply from server and print them to Serial.
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
    WUResult = line;
    yield();
  }
  Serial.print("WU Result: ");
  Serial.println(WUResult);

  client.stop();

  if (WUResult.indexOf("success") != -1) {
    Serial.println("WU: Data send successful.");
    return 1;
  } else if(WUResult.indexOf("INVALIDPASSWORDID") != -1) {
    Serial.println("WU: Station Key and/or ID are incorrect.");
    return 1;
  } else {
    Serial.println("WU: Data send failed.");
  }
  return 0;
}
