void handleRoot() {
  server.sendHeader("Location", "/index.html", true);  //Redirect to our html web page
  server.send(302, "text/plane", "");
}

bool loadFromSpiffs(String path) {
  Serial.print("path ist ");
  String dataType = "text/plain";
  if (path.endsWith("/"))
    path += "index.htm";
  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  Serial.println(path);
  return true;
}

void handleWebRequests() {
  if (loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void handleLED() {
  static int Counter = 0;
  String TickTack;
  Counter++;
  String LEDValue = "<img src=\"/Blank.png\" alt=\"Blank\" width=\"60\" height=\"30\"> <img src=\"/";
  if (Counter & 1) {
    LEDValue += "LED-R.png";
  }
  else {
    LEDValue += "LED-G.png";
  }
  LEDValue += "\" alt=\"Tick\" width=\"60\" height=\"60\">";
  LEDValue += "<img src=\"/Blank.png\" alt=\"Blank\" width=\"30\" height=\"30\">";
  LEDValue += " <img src=\"/";
  if (Counter & 1) {
    LEDValue += "LED-G.png";
  }
  else {
    LEDValue += "LED-R.png";
  }
  LEDValue += "\" alt=\"Tack\" width=\"60\" height=\"60\"> </h2>";
  server.send(200, "text/plane", LEDValue); //Send ADC value only to client ajax request
}

void handleJSON(void) {
  static int CallCounter = 0;
  time_t renteFix = 1919635200;
  const size_t capacity = JSON_OBJECT_SIZE(18);
  DynamicJsonDocument doc(capacity);
  char JSONBuffer[512];
  String strVorname, strNachname;
  EEPROM.get(0, Daten);
  strVorname = Daten.vorname;
  strNachname = Daten.nachname;

  doc["compile"] = __DATE__ " " __TIME__;
  doc["vorname"] = strVorname;
  doc["nachname"] = strNachname;
  doc["foermlich"] = Daten.foermlich;
  doc["geschlecht"] = Daten.geschlecht;
  doc["vcc"] = ESP.getVcc();
  doc["wochentag"] = Wochentag[weekday(now())];
  doc["tag"] = MakeDateString(now());
  doc["uhrzeit"] = MakeTimeString(now());
  doc["rentein"] = (Daten.rente - now()) / 86400;
  doc["rente"] = Daten.rente;
  doc["helligkeit"] = 2103.4;
  doc["temperatur"] = 21.3;
  doc["feuchte"] = 56.7;
  doc["druck"] = 1013.5;

  serializeJson(doc, JSONBuffer);
  server.send(200, "text/plane", JSONBuffer); //Send JSON Data to client ajax request
}

void readJSON() {
  DynamicJsonDocument doc(512);

  if (server.hasArg("plain") == false) { //Check if body received
    server.send(200, "text/plain", "JSON not received");
    Serial.println("Nix empfangen");
    return;
  }
  server.send(200, "text/plain", "JSON received");
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  GetEE();
  strcpy(Daten.vorname, doc["vorname"]);
  strcpy(Daten.nachname, doc["nachname"]);
  Daten.foermlich = doc["foermlich"];
  Daten.geschlecht = doc["geschlecht"];
  Daten.rente = doc["rente"];
  /*Serial.println("Daten von Client empfangen:");
    Serial.println(Daten.vorname);
    Serial.println(Daten.nachname);
    Serial.println(Daten.foermlich);
    Serial.println(Daten.geschlecht);*/
  Serial.println(Daten.rente);
  PutEE();
  Serial.println("Daten gespeichert");
}
