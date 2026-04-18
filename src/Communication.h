#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include "constants.h"
#include "WiFiS3.h"
#include "ArduinoJson.h"
#include "secrets.h"
#include "ArduinoMqttClient.h"
#include "RTC.h"
#include "NTPClient.h"

class Communication
{
private:
    WiFiClient espClient;
    MqttClient client = MqttClient(espClient);
    WiFiUDP Udp;
    NTPClient timeClient = NTPClient(Udp);
    bool _isDynamicPanel;

public:
    int setup(bool isDynamicPanel);
    void poll();
    int sendDataDynamic(double temp, double humidity, double tl_ldr,
                        double bl_ldr, double tr_ldr, double br_ldr, double az,
                        double ze, double panelV, double panelA, double systemV, double systemA);
    int sendDataStatic(double temp, double humidity, double tl_ldr,
                       double bl_ldr, double tr_ldr, double br_ldr, double panelV, double panelA);
    int sendLog(char *buffer);
    unsigned long getTime();
};

#endif