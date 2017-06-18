#include <ESP8266WiFi.h>
#define SOFTAPPRE "WAT"

#define SSID_SIZE 32
#define PASS_SIZE 64

extern char softAPssID[SSID_SIZE];
extern char softAPpass[PASS_SIZE];


/* Soft AP network parameters */
extern IPAddress apIP;
extern IPAddress netMsk;

/* STA params */
extern char STAssID[SSID_SIZE];
extern char STApass[PASS_SIZE];
