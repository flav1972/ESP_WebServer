#include <ESP8266WiFi.h>
#include "params.h"

char softAPssID[SSID_SIZE];
char softAPpass[PASS_SIZE];

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

/* STA params */
char STAssID[SSID_SIZE];
char STApass[PASS_SIZE];
