/*
 * Test of Webserver with data storage as STA (station) and AP (AccessPoint)
 * Based on examples from
 * CaptivePortalAdvanced: https://github.com/esp8266/Arduino/blob/master/libraries/DNSServer/examples/CaptivePortalAdvanced/CaptivePortalAdvanced.ino
 * AutoConnectWithFSParameters: https://github.com/tzapu/WiFiManager/tree/master/examples/AutoConnectWithFSParameters
 * WifiScan: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiScan/WiFiScan.ino
 *
 */
#include <Arduino.h>
#include <Ticker.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include "globals.h"
#include "flip.h"
#include "params.h"
#include "server.h"
extern void attach_events(); // in events.cpp

bool re_connect = false;

void PrintStatus() {
  int s;

  Serial.print("WiFi Client status: ");
  s = WiFi.status();
  Serial.print(s);
  Serial.print("=");
  switch(s) {
    case WL_NO_SHIELD:
      Serial.println("WL_NO_SHIELD");
      break;
    case WL_IDLE_STATUS:
      Serial.println("WL_IDLE_STATUS");
      break;
    case WL_NO_SSID_AVAIL:
      Serial.println("WL_NO_SSID_AVAIL");
      break;
    case WL_SCAN_COMPLETED:
      Serial.println("WL_SCAN_COMPLETED");
      break;
    case WL_CONNECTED:
      Serial.println("WL_CONNECTED");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("WL_CONNECT_FAILED");
      break;
    case WL_CONNECTION_LOST:
      Serial.println("WL_CONNECTION_LOST");
      break;
    case WL_DISCONNECTED:
      Serial.println("WL_DISCONNECTED");
      break;
    default:
      Serial.println("Other");
  }
}

void setup() {
  String myMAC;
  uint8_t mac[6];
  char macStr[7];
  uint32_t chipId;

  Serial.begin(115200);
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, LOW);
  flipper.attach(0.3, flip); // defined in flip

  Serial.println();

  /* Attach Wifi Events */
  attach_events();

  /* Get and display Mac adress */
	Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP_STA); // acces point plus station

  myMAC = WiFi.softAPmacAddress();
  WiFi.softAPmacAddress(mac);
  sprintf(macStr, "%02X%02X%02X", mac[3], mac[4], mac[5]);
  Serial.print("My AP MAC : "); Serial.println(myMAC);
  Serial.print("Last Mac part: "); Serial.println(macStr);

  chipId = ESP.getChipId();
  Serial.print("Chip ID: ");
  Serial.println(chipId);

  /* create softAP ssid */
  softAPssID[0] = 0;
  strcat(softAPssID, SOFTAPPRE);
  strcat(softAPssID, macStr);

  /* Define IP of SoftAP */
  WiFi.softAPConfig(apIP, apIP, netMsk);

  /* Start SoftAP */
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(softAPssID/*, password */);

  delay(500); // be sure softAP is on

  IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);


  Serial.print("Starting Webserver...");
  // config and start Webserver
  webserver_config();
  Serial.println("Done");

  PrintStatus();

}

void loop() {
  //Serial.print("Hello World! ");
  //Serial.print("State = ");
  //Serial.println(gstate);
  server.handleClient();
  if(re_connect) {
    re_connect = false;
    Serial.print("Changed STA SSID: ");
    Serial.print(STAssID);
    Serial.print("/");
    Serial.println(STApass);
    WiFi.begin(STAssID, STApass);
  }
  yield();
}
