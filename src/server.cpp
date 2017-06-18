#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "globals.h"
#include "params.h"
#include "server.h"

#define DEBUG_WM Serial.println
const bool _removeDuplicateAPs = false;
const bool _minimumQuality = false;

int getRSSIasQuality(int RSSI) {
  int quality = 0;

  if (RSSI <= -100) {
    quality = 0;
  } else if (RSSI >= -50) {
    quality = 100;
  } else {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}


// Web server
ESP8266WebServer server(80);

void handleRoot()
{
  char temp[400];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

  Serial.println("handleRoot");

/*
	snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
  </body>\
</html>",

		hr, min % 60, sec % 60
	);
*/
  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Home");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  //page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("<h1>Flav Test</h1>");
  page += F("<h3>WiFiManager</h3>");
  page += FPSTR(HTTP_PORTAL_OPTIONS);
  snprintf(temp, 400, "<p>Uptime: %02d:%02d:%02d</p>",
    hr, min % 60, sec % 60);
  page += temp;
  page += FPSTR(HTTP_END);

  server.send(200, "text/html", page);
}

/** Wifi config page handler */
void handleWifi(boolean scan) {

  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Config ESP");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
//  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);

  if (scan) {
    int n = WiFi.scanNetworks();
    DEBUG_WM(F("Scan done"));
    if (n == 0) {
      DEBUG_WM(F("No networks found"));
      page += F("No networks found. Refresh to scan again.");
    } else {

      //sort networks
      int indices[n];
      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }

      // RSSI SORT

      // old sort
      for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
          if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
            std::swap(indices[i], indices[j]);
          }
        }
      }

      /*std::sort(indices, indices + n, [](const int & a, const int & b) -> bool
        {
        return WiFi.RSSI(a) > WiFi.RSSI(b);
        });*/

      // remove duplicates ( must be RSSI sorted )
      if (_removeDuplicateAPs) {
        String cssid;
        for (int i = 0; i < n; i++) {
          if (indices[i] == -1) continue;
          cssid = WiFi.SSID(indices[i]);
          for (int j = i + 1; j < n; j++) {
            if (cssid == WiFi.SSID(indices[j])) {
              DEBUG_WM("DUP AP: " + WiFi.SSID(indices[j]));
              indices[j] = -1; // set dup aps to index -1
            }
          }
        }
      }

      //display networks in page
      for (int i = 0; i < n; i++) {
        if (indices[i] == -1) continue; // skip dups
        DEBUG_WM(WiFi.SSID(indices[i]));
        DEBUG_WM(WiFi.RSSI(indices[i]));
        int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

        if (_minimumQuality == -1 || _minimumQuality < quality) {
          String item = FPSTR(HTTP_ITEM);
          String rssiQ;
          rssiQ += quality;
          item.replace("{v}", WiFi.SSID(indices[i]));
          item.replace("{r}", rssiQ);
          if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) {
            item.replace("{i}", "l");
          } else {
            item.replace("{i}", "");
          }
          //DEBUG_WM(item);
          page += item;
          delay(0);
        } else {
          DEBUG_WM(F("Skipping due to quality"));
        }

      }
      page += "<br/>";
    }
  }

  page += FPSTR(HTTP_FORM_START);
  char parLength[2];
/*
  // add the extra parameters to the form
  for (int i = 0; i < _paramsCount; i++) {
    if (_params[i] == NULL) {
      break;
    }

    String pitem = FPSTR(HTTP_FORM_PARAM);
    if (_params[i]->getID() != NULL) {
      pitem.replace("{i}", _params[i]->getID());
      pitem.replace("{n}", _params[i]->getID());
      pitem.replace("{p}", _params[i]->getPlaceholder());
      snprintf(parLength, 2, "%d", _params[i]->getValueLength());
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", _params[i]->getValue());
      pitem.replace("{c}", _params[i]->getCustomHTML());
    } else {
      pitem = _params[i]->getCustomHTML();
    }

    page += pitem;
  }
  if (_params[0] != NULL) {
    page += "<br/>";
  }

  if (_sta_static_ip) {

    String item = FPSTR(HTTP_FORM_PARAM);
    item.replace("{i}", "ip");
    item.replace("{n}", "ip");
    item.replace("{p}", "Static IP");
    item.replace("{l}", "15");
    item.replace("{v}", _sta_static_ip.toString());

    page += item;

    item = FPSTR(HTTP_FORM_PARAM);
    item.replace("{i}", "gw");
    item.replace("{n}", "gw");
    item.replace("{p}", "Static Gateway");
    item.replace("{l}", "15");
    item.replace("{v}", _sta_static_gw.toString());

    page += item;

    item = FPSTR(HTTP_FORM_PARAM);
    item.replace("{i}", "sn");
    item.replace("{n}", "sn");
    item.replace("{p}", "Subnet");
    item.replace("{l}", "15");
    item.replace("{v}", _sta_static_sn.toString());

    page += item;

    page += "<br/>";
  }
*/

  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_SCAN_LINK);

  page += FPSTR(HTTP_END);

  server.send(200, "text/html", page);

  Serial.println(F("Sent config page"));
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  DEBUG_WM(F("WiFi save"));

  //SAVE/connect here
  strncpy(STAssID, server.arg("s").c_str(), SSID_SIZE);
  strncpy(STApass, server.arg("p").c_str(), PASS_SIZE);

/*
  //parameters
  for (int i = 0; i < _paramsCount; i++) {
    if (_params[i] == NULL) {
      break;
    }
    //read parameter
    String value = server->arg(_params[i]->getID()).c_str();
    //store it in array
    value.toCharArray(_params[i]->_value, _params[i]->_length);
    DEBUG_WM(F("Parameter"));
    DEBUG_WM(_params[i]->getID());
    DEBUG_WM(value);
  }
*/

/*
  if (server->arg("ip") != "") {
    DEBUG_WM(F("static ip"));
    DEBUG_WM(server->arg("ip"));
    //_sta_static_ip.fromString(server->arg("ip"));
    String ip = server->arg("ip");
    optionalIPFromString(&_sta_static_ip, ip.c_str());
  }
  if (server->arg("gw") != "") {
    DEBUG_WM(F("static gateway"));
    DEBUG_WM(server->arg("gw"));
    String gw = server->arg("gw");
    optionalIPFromString(&_sta_static_gw, gw.c_str());
  }
  if (server->arg("sn") != "") {
    DEBUG_WM(F("static netmask"));
    DEBUG_WM(server->arg("sn"));
    String sn = server->arg("sn");
    optionalIPFromString(&_sta_static_sn, sn.c_str());
  }
*/
  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Credentials Saved");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
//  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += FPSTR(HTTP_SAVED);
  page += FPSTR(HTTP_END);

  server.send(200, "text/html", page);

  DEBUG_WM(F("Sent wifi save page"));

  re_connect = true; //signal ready to connect/reset
}

/** Handle the info page */
void handleInfo() {
  DEBUG_WM(F("Info"));

  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
//  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("<dl>");
  page += F("<dt>Chip ID</dt><dd>");
  page += ESP.getChipId();
  page += F("</dd>");
  page += F("<dt>Flash Chip ID</dt><dd>");
  page += ESP.getFlashChipId();
  page += F("</dd>");
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</dd>");
  page += F("<dt>Real Flash Size</dt><dd>");
  page += ESP.getFlashChipRealSize();
  page += F(" bytes</dd>");
  page += F("<dt>Soft AP IP</dt><dd>");
  page += WiFi.softAPIP().toString();
  page += F("</dd>");
  page += F("<dt>Soft AP MAC</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  page += F("</dl>");
  page += FPSTR(HTTP_END);

  server.send(200, "text/html", page);

  DEBUG_WM(F("Sent info page"));
}

/** Handle the reset page */
void handleReset() {
  DEBUG_WM(F("Reset"));

  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
//  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("Module will reset in a few seconds.");
  page += FPSTR(HTTP_END);
  server.send(200, "text/html", page);

  DEBUG_WM(F("Sent reset page"));
  delay(5000);
  ESP.reset(); // hardware reset
//  ESP.restart(); // sdk reset
  delay(2000);
}

void webserver_config()
{
  server.on("/", handleRoot);
  server.on("/wifi", std::bind(handleWifi, true));
  server.on("/0wifi", std::bind(handleWifi, false));
  server.on("/wifisave", handleWifiSave);
  server.on("/i", handleInfo);
  server.on("/r", handleReset);
  server.onNotFound([](){
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin();
}
