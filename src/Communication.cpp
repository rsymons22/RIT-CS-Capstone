#include "Communication.h"
void Communication::poll()
{
    client.poll();
}

unsigned long Communication::getTime()
{
    timeClient.update();
    return timeClient.getEpochTime();
}

int Communication::sendDataDynamic(double temp, double humidity, double tl_ldr, double bl_ldr, double tr_ldr, double br_ldr, double az, double ze, double panelV, double panelA, double systemV, double systemA)
{
    JsonDocument doc;

    doc["temp"] = temp;
    doc["humidity"] = humidity;
    doc["tl_ldr"] = tl_ldr;
    doc["bl_ldr"] = bl_ldr;
    doc["tr_ldr"] = tr_ldr;
    doc["br_ldr"] = br_ldr;
    doc["azimuth"] = az;
    doc["zenith"] = ze;
    doc["panelV"] = panelV;
    doc["panelA"] = panelA;
    doc["systemV"] = systemV;
    doc["systemA"] = systemA;

    client.beginMessage("solar/dynamic");
    serializeJson(doc, client);
    return client.endMessage();
}

int Communication::sendDataStatic(double temp, double humidity, double tl_ldr, double bl_ldr, double tr_ldr, double br_ldr, double panelV, double panelA)
{
    JsonDocument doc;

    doc["temp"] = temp;
    doc["humidity"] = humidity;
    doc["tl_ldr"] = tl_ldr;
    doc["bl_ldr"] = bl_ldr;
    doc["tr_ldr"] = tr_ldr;
    doc["br_ldr"] = br_ldr;
    doc["panelV"] = panelV;
    doc["panelA"] = panelA;

    client.beginMessage("solar/static");
    serializeJson(doc, client);
    return client.endMessage();
}

int Communication::sendLog(char *buffer)
{
    JsonDocument doc;

    doc["system"] = _isDynamicPanel;
    doc["message"] = buffer;

    client.beginMessage("solar/logger");
    serializeJson(doc, client);
    return client.endMessage();
}

int Communication::setup(bool isDynamicPanel)
{
    _isDynamicPanel = isDynamicPanel;
    WiFi.begin(SECRET_SSID, SECRET_PASS);

    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        if (attempts == 3)
            return 1;
        attempts++;
    }

    attempts = 0;

    while (!client.connect(RASPBERRYPI_IP, COMM_PORT))
    {
        delay(500);

        if (attempts == 3)
            return 2;
        attempts++;
    }

    timeClient.begin();

    return 0;
}