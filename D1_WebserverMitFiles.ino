/*
 * Kompilieren mit 256kB SPIFFS
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <FS.h>   //Include File System Headers
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUDP.h>
#include <TimeLib.h>
#include <Timezone.h>     // from https://github.com/JChristensen/Timezone
#include <ArduinoJson.h>

ADC_MODE(ADC_VCC);

#define SYNC_INTERVAL 1800  //alle 30min Zeit synchronisieren
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone LTZ(CEST, CET);    // this is the Timezone object that will be used to calculate local time
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
#define localPort 2390
// A UDP instance to let us send and receive packets over UDP
const long utcOffsetInSeconds = 0;
unsigned long UpdateIntervall = 1800;
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", utcOffsetInSeconds, UpdateIntervall);

ESP8266WebServer server(80); //Server on port 80

tmElements_t TP;

const char* Wochentag[8] = {
  "Fehler",
  "Sonntag",
  "Montag",
  "Dienstag",
  "Mittwoch",
  "Donnerstag",
  "Freitag",
  "Samstag"
};


IPAddress ip;
time_t LastShow = 0, LastSync = 0;

struct EEData_t{
  int check;
  char vorname[64];
  char nachname[64];
  time_t rente;
  int foermlich;
  int geschlecht;
} ;

EEData_t Daten;


void setup() {
  SPIFFS.begin();
  EEPROM.begin(200);
  Serial.begin(115200);
  Serial.println();
  Serial.println("--- Geht los ---");
  Serial.println("");
  WiFiManager wifiManager;
  uint8_t macAddr[6];
  char Hostname[15];
  WiFi.macAddress(macAddr);
  sprintf(Hostname, "ottO-%02x-%02x-%02x", macAddr[3], macAddr[4], macAddr[5]);
  WiFi.hostname(Hostname);
  /*if (!digitalRead(startAP))
    wifiManager.resetSettings();*/
  wifiManager.autoConnect("WebTest-AP");

  // Wait for connection
  ip = WiFi.localIP();
  Serial.print("IP address: ");
  Serial.println(ip);  //IP address assigned to your ESP
  WiFi.printDiag(Serial);
  udp.begin(localPort);
  delay(100);
  setSyncProvider(SyncTimeToNTP);
  setSyncInterval(1800);
  LastSync = now();
  InitEE();
  DefServerFunctions();
  server.begin();
}

void loop() {
  server.handleClient();

}
