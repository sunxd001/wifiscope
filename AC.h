#ifndef ac_H_
#define ac_H_

#define LOG_ENABLE
//#define UPDATE_IP   //if show config
//#define LOOP_10_S 2800000
#define LOOP_10_S 2800
#define MEM_SAVED   0xAB

#define PGM_P       const char *
#define PGM_VOID_P  const void *
#define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))

//0-161 is allocated for AutoConnect (162 bytes)
//162-255 is free for the program (94 bytes)
#define EEP_AC_KEY        0
#define EEP_ST_SSID       1
#define EEP_ST_PASS       33
#define EEP_AP_SSID       65
#define EEP_AP_PASS       97
#define EEP_SECURE        129
#define EEP_AC_CONFIG     161

#define EEP_ST_SSID_LEN   32
#define EEP_ST_PASS_LEN   32
#define EEP_AP_SSID_LEN   32
#define EEP_AP_PASS_LEN   32
#define EEP_SECURE_LEN    32

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "EEPROM.h"

ESP8266WebServer  Server;

bool DisableHotSpot;

char STSsid[EEP_ST_SSID_LEN]; //1-32
char STPassword[EEP_ST_PASS_LEN]; //33-64
char APSsid[EEP_AP_SSID_LEN]; //65-96
char APPassword[EEP_AP_PASS_LEN]; //97-128
char secureLink[EEP_SECURE_LEN]; //129-160

void configPage_AC();
void init_AC();
void handle_AC();

#endif // INCLUDED
