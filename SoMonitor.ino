#define API_ENDPOINT "http://reapistaging.altervista.org/sensor_update.php"

#define AP_SSID1 "SSID1"
#define AP_PASS1 "PASS1"

#define AP_SSID2 "Lab104@Sobrero"
#define AP_PASS2 "-------"

#define AP_SSID3 "SoMonitor Debug"
#define AP_PASS3 "S0br3r0"

#define CON_TIMEOUT 10000
#define HEARTBEAT 30000

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti wifi;

int startTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\nSoMonitor Versione v1.0");
  Serial.print("Compilata il ");
  Serial.println(__DATE__);
  Serial.print("Identificatore sensore: ");
  Serial.println(ESP.getChipId(), HEX);

  Serial.print("Inizializzando l'interfaccia di rete...");
  WiFi.forceSleepWake();
  yield();
  WiFi.mode(WIFI_STA);
  Serial.println("\t[ OK ]");

  Serial.print("Configurando l'interfaccia di rete...");
  wifi.addAP(AP_SSID1, AP_PASS1);
  wifi.addAP(AP_SSID2, AP_PASS2);
  wifi.addAP(AP_SSID3, AP_PASS3);
  Serial.println("\t[ OK ]");

  Serial.print("Connessione al network IoT in corso...");
  startTime = millis();
  while (wifi.run() != WL_CONNECTED) {
    if (millis() > (startTime + CON_TIMEOUT)) {
      Serial.println("[ FAIL ]");
      Serial.println("Impossibile connettersi al network IoT, processore arrestato");
      ESP.wdtDisable(); // disabilita il timer watchdog
      while (1);
    }
    yield(); // resetta il timer watchdog (almeno non si resetta niente)
  }
  Serial.println("\t[ OK ]");
  Serial.print("Indirizzo IP acquisito dal DHCP: ");
  Serial.println(WiFi.localIP());
  Serial.println("-- SOMONITOR IN FUNZIONE --");
}

void loop() {
  char request[255];
  sprintf(request, "%s/?id=%06X&value=%f&status=ok&rssi=%ld", API_ENDPOINT, ESP.getChipId(), 12.3, WiFi.RSSI());

  WiFiClient client;
  HTTPClient http;
  Serial.print("Inizio sessione HTTP in corso...");
  if (http.begin(client, request)) {
    Serial.println("\t[ OK ]");
    Serial.print("Invio richiesta HTTP in corso...");
    if (http.GET() == 200) {
      Serial.println("\t[ OK ]");
    } else {
      Serial.println("\t[ FAIL ]");
    }
    http.end();
  } else {
    Serial.println("\t[ FAIL ]");
  }
  Serial.println("\nAvvio modalità sleep, reset dopo heartbeat");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(HEARTBEAT);
  ESP.restart();
}
