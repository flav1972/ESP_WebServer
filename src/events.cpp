#include <Arduino.h>
#include <ESP8266WiFi.h>

String macToString(const unsigned char*);

WiFiEventHandler onStationModeConnectedO;
WiFiEventHandler onStationModeDisconnectedO;
WiFiEventHandler onStationModeAuthModeChangedO;
WiFiEventHandler onStationModeGotIPO;
WiFiEventHandler onStationModeDHCPTimeoutO;
WiFiEventHandler onSoftAPModeStationConnectedO;
WiFiEventHandler onSoftAPModeStationDisconnectedO;
WiFiEventHandler onSoftAPModeProbeRequestReceivedO;

String bssidToString(const uint8 *bssid) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
  return String(buf);
}

void onStationModeConnectedH(const WiFiEventStationModeConnected& evt) {
  Serial.print("Station connected: SSID=");
  Serial.print(evt.ssid);
  Serial.print(" channel=");
  Serial.print(evt.channel);
  Serial.print(" bssid=");
  Serial.println(bssidToString(evt.bssid));
}

void onStationModeDisconnectedH(const WiFiEventStationModeDisconnected& evt) {
  Serial.print("Station Disconnected: SSID=");
  Serial.print(evt.ssid);
  Serial.print(" reason=");
  Serial.print(evt.reason);
  Serial.print(" bssid=");
  Serial.println(bssidToString(evt.bssid));
}

void onStationModeAuthModeChangedH(const WiFiEventStationModeAuthModeChanged& evt) {
  Serial.print("Station AuthModeChanged oldMode: ");
  Serial.print(evt.oldMode);
  Serial.print(" newMode: ");
  Serial.println(evt.newMode);
}

void onStationModeGotIPH(const WiFiEventStationModeGotIP& evt) {
  Serial.print("Station got IP: ");
  Serial.print(evt.ip);
  Serial.print("/");
  Serial.print(evt.mask);
  Serial.print(" gw ");
  Serial.println(evt.gw);
}

void onStationModeDHCPTimeoutH(void) {
  Serial.println("Station DHCP Timeout");
}

void onSoftAPModeStationConnectedH(const WiFiEventSoftAPModeStationConnected& evt) {
  Serial.print("SoftAP Station Connected: ");
  Serial.print(macToString(evt.mac));
  Serial.print(" aid: ");
  Serial.println(evt.aid);
}

void onSoftAPModeStationDisconnectedH(const WiFiEventSoftAPModeStationDisconnected& evt) {
  Serial.print("SoftAP Station Disconnected: ");
  Serial.println(macToString(evt.mac));
  Serial.print("aid: ");
  Serial.println(evt.aid);
}

void onSoftAPModeProbeRequestReceivedH(const WiFiEventSoftAPModeProbeRequestReceived& evt) {
  Serial.print("SoftAP Probe request from: ");
  Serial.print(macToString(evt.mac));
  Serial.print(" RSSI: ");
  Serial.println(evt.rssi);
}

void attach_events() {
  // Register event handlers.
  // Callback functions will be called as long as these handler objects exist.
  // Call "onStationConnected" each time a station connects
  onStationModeConnectedO = WiFi.onStationModeConnected(onStationModeConnectedH);
  onStationModeDisconnectedO = WiFi.onStationModeDisconnected(onStationModeDisconnectedH);
  onStationModeAuthModeChangedO = WiFi.onStationModeAuthModeChanged(onStationModeAuthModeChangedH);
  onStationModeGotIPO = WiFi.onStationModeGotIP(onStationModeGotIPH);
  onStationModeDHCPTimeoutO = WiFi.onStationModeDHCPTimeout(onStationModeDHCPTimeoutH);
  onSoftAPModeStationConnectedO = WiFi.onSoftAPModeStationConnected(onSoftAPModeStationConnectedH);
  onSoftAPModeStationDisconnectedO = WiFi.onSoftAPModeStationDisconnected(onSoftAPModeStationDisconnectedH);
}

String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}
