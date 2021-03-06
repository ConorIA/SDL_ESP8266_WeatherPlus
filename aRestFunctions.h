//
//
//
// aRest Command functions
//
//

// #include "OLEDDisplay.h";
void displayQueueScreens(int format);
void displayConsoleClear();

int ledControl(String command) {
  // Get state from command.
  int state = command.toInt();

  // ESP8266 inverts sense (HIGH = off, LOW = ON).
  if (state == 0) {
    state = 1;
  } else {
    state = 0;
  }

  digitalWrite(blinkPin, state);
  return 1;
}

int setWeatherPlusIDControl(String command) {
  rest.set_id(command);
  return 1;
}

int resetWiFiAccessPoint(String command) {
  // Example: http://192.168.1.NNN/resetWiFiAccessPoint?params=adminpassword
  Serial.print("Command =");
  Serial.println(command);

  if (command == adminPassword) {
    WPassword = "XXX";
    Wssid = "XXX";
    writeEEPROMState();
    return 1;
  } else {
    return 0;
  }

  return 1;
}

int resetOurWeather(String command) {
  Serial.println("resetOurWeather - settings invalidated");
  Serial.print("Command =");
  Serial.println(command);

  if (command == adminPassword) {
    invalidateEEPROMState();
    system_restart();
    // qdelay(10000);
    return 1;
  }

  return 0;
}

int setAdminPassword(String command) {
  // Example: http://192.168.1.NNN/setAdminPassword?params=oldpassword,newpassword
  Serial.print("Command =");
  Serial.println(command);

  String oldPassword;
  String newPassword;

  oldPassword = getValue(command, ',', 0);
  newPassword = getValue(command, ',', 1);

  if (oldPassword == adminPassword) {
    adminPassword = newPassword;
    writeEEPROMState();
    return 1;
  } else {
    return 0;
  }

  return 1;
}

int setWUSID(String command) {
  // Example: http://192.168.1.NNN/setWUSID?params=AdminPassword,WUStationID
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String WUSID;

  sentPassword = getValue(command, ',', 0);
  WUSID = getValue(command, ',', 1);

  Serial.print("adminPassword=");
  Serial.println(adminPassword);
  Serial.print("sentPassword=");
  Serial.println(sentPassword);

  if (sentPassword == adminPassword) {
    WeatherUnderground_StationID = WUSID;
    writeEEPROMState();
    return 1;
  } else {
    return 0;
  }

  return 1;
}

int setWUKEY(String command) {
  // Example: http://192.168.1.NNN/setWUKEY?params=AdminPassword,WUStationID
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String WUKEY;

  sentPassword = getValue(command, ',', 0);
  WUKEY = getValue(command, ',', 1);

  if (sentPassword == adminPassword) {
    WeatherUnderground_StationKey = WUKEY;
    writeEEPROMState();
    return 1;
  } else {
    return 0;
  }

  return 1;
}

void startBlynk();

int setBAKEY(String command) {
  // Example: http://192.168.1.NNN/setBAKEY?params=AdminPassword,BlynkKey
  Serial.print("Command =");
  Serial.println(command);

  String sentPassword;
  String BAKEY;

  sentPassword = getValue(command, ',', 0);
  BAKEY = getValue(command, ',', 1);

  if (sentPassword == adminPassword) {
    BlynkAuthCode = BAKEY;
    writeEEPROMState();
    startBlynk();
    return 1;
  } else {
    return 0;
  }

  return 1;
}

int rebootOurWeather(String command) {
  system_restart();
  // ESP.reset();
  delay(10000);

  return 1;
}

int setDateTime(String command) {
  String _time;
  String _date;
  String _password;

  _password = getValue(command, ',', 0);
  _date = getValue(command, ',', 1);
  _time = getValue(command, ',', 2);

  Serial.println("command values:");
  Serial.println(_password);
  Serial.println(_date);
  Serial.println(_time);

  _date.replace("%20", " ");
  Serial.println(_password);
  Serial.println(_date);
  Serial.println(_time);

  if (_password == adminPassword) {
    if ((_time.length() > 0) && (_date.length() > 0)) {
      RtcDateTime compiled = RtcDateTime(_date.c_str(), _time.c_str());
      if (compiled.IsValid()) {
        Serial.println("Updating DateTime in RTC");
        Serial.println(compiled);
        Rtc.SetDateTime(compiled);
        return 1;
      }
    }
    // Password valid but date/time is not.
    Serial.println("Provided date/time is not valid. Not updating.");
    return 2;
  } else {
    return 0;
  }
}

// FOTA update commands

int updateOurWeather(String command) {
  Serial.println("updateOurWeather");
  Serial.print("Command =");
  Serial.println(command);
  if (command == adminPassword) {
    // Previously: updateDisplay (DISPLAY_UPDATING);
    displayConsoleClear();
    displayConsolePrint("OurWeather Updating");

    // TODO: What was this supposed to display. Does not make sense.
    // updateDisplay(DISPLAY_UPDATE_FINISHED);

    t_httpUpdate_return ret = ESPhttpUpdate.update("www.switchdoc.com", 80,
        "/OurWeatherUpdater.php", WEATHERPLUSESP8266VERSION);

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.println("[update] Update failed.");
        // Previously: updateDisplay (DISPLAY_NO_UPDATE_FAILED);
        displayConsolePrint("Update failed.", false);
        displayConsolePrint("Try again later.");
        delay(1000);
        return 1;
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Updates.");
        // Previously:  updateDisplay (DISPLAY_NO_UPDATE_AVAILABLE);
        displayConsolePrint("You have the most", false);
        displayConsolePrint("recent software.");
        delay(1000);
        return 2;
        break;

      case HTTP_UPDATE_OK:
        // TODO: Fix comment: what is this doing?
        // Original comment: May not called we reboot the ESP.
        Serial.println("[update] Update ok.");
        // Previously: NO OUTPUT
        // Added output of updateDisplay(DISPLAY_UPDATE_FINISHED) which made most sense.
        displayConsolePrint("OurWeather Updated", false);
        displayConsolePrint("Wait 120 Seconds", false);
        displayConsolePrint("Unplug Power", false);
        displayConsolePrint("Wait 15 Seconds", false);
        displayConsolePrint("Plug Power In");
        return 3;
        break;
    }
  }

  return 0;
}

// Connection commands.

int resetToDefaults(String command) {
  return 1;
}

int enableCWOPControl(String command) {
  // Get state from command.
  int state = command.toInt();
  return 1;
}

int enableTwitterControl(String command) {
  // Get state from command.
  int state = command.toInt();
  return 1;
}

// Weather Controls.
int weatherSmallControl(String command) {
  WeatherDisplayMode = DISPLAY_WEATHER_SMALL;
  writeEEPROMState();
  displayQueueScreens(WeatherDisplayMode);
  return 1;
}

int weatherMediumControl(String command) {
  WeatherDisplayMode = DISPLAY_WEATHER_MEDIUM;
  writeEEPROMState();
  displayQueueScreens(WeatherDisplayMode);
  return 1;
}

int weatherLargeControl(String command) {
  WeatherDisplayMode = DISPLAY_WEATHER_LARGE;
  writeEEPROMState();
  displayQueueScreens(WeatherDisplayMode);
  return 1;
}

int weatherDemoControl(String command) {
  WeatherDisplayMode = DISPLAY_WEATHER_DEMO;
  writeEEPROMState();
  displayQueueScreens(WeatherDisplayMode);
  return 1;
}

void writeToBlynkStatusTerminal(String statement);

int englishUnitControl(String command) {
  EnglishOrMetric = 0;
  writeToBlynkStatusTerminal("Units set to English");
  Blynk.virtualWrite(V8, "English");
  writeEEPROMState();
  user_units = UK;
  return 1;
}

int metricUnitControl(String command) {
  EnglishOrMetric = 1;
  writeToBlynkStatusTerminal("Units set to Metric");
  Blynk.virtualWrite(V8, "Metric");
  writeEEPROMState();
  user_units = SI;
  return 1;
}

int usaUnitControl(String command) {
  EnglishOrMetric = 2;
  writeToBlynkStatusTerminal("Units set to USA");
  Blynk.virtualWrite(V8, "USA");
  writeEEPROMState();
  user_units = USA;
  return 1;
}

// RasPiConnect.
int jsonCmd(String command) {
  Serial.println("in jsonCmd");
  return 1;
}

/*
void jsonCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  currentObjectStructure parsedObject;
  initParsedObject(&parsedObject);

#ifdef DEBUG
  Serial.println("------------------");
#endif

  char myBuffer[1024];

  server.httpSuccess("application/json");

  if (type == WebServer::HEAD) {
    return;
  }

  int myChar;
  int count;
  myChar = server.read();
  count = 0;

  while (myChar > 0) {
    myBuffer[count] = (char) myChar;
    myChar = server.read();
    count++;
  }
  myBuffer[count] = '\0';

  delay(25);

  messageCount++;

  char returnJSON[500] = "";

  ExecuteCommandAndReturnJSONString(myBuffer, messageCount, md5str, &parsedObject, returnJSON, returnJSON);

#ifdef DEBUG
  Serial.print("returnJSON =");
  Serial.println(returnJSON);
#endif

  server << returnJSON;

#ifdef DEBUG
  Serial.print("Mem1=");
  Serial.println(freeMemory());
  Serial.println("------------------");
#endif

}
*/
