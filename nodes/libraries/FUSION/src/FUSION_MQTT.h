#ifndef FUSION_MQTT_H
#define FUSION_MQTT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "FUSION_UTILS.h"

// TODO why is this here and in FUSION_MODULE?
#include <stdlib.h>
#include "Adafruit_Sensor.h"
#include <DHT.h>
#include <Wire.h>
#include <map>

class FusionMQTT
{
    public:
        FusionMQTT();
        char* topic_network;
        char* topic_location;
        char* topic_name;
        void init();
        void send(uint16_t data, char* topic_data);
        void send(uint8_t* data, unsigned int length, char* topic_data);
        void send(char* data, unsigned int length, char* topic_data);
        void send(const char* data, char* topic_data);
        void send(const char* data, char* topic_data, bool retain);
        void update();

        void callback(char* topic, uint8_t* payload, unsigned int length);

        void registerCallback(std::function<void(char*, uint8_t*, unsigned int)> callback_function, char* topic);

    private:
        WiFiClient wifiClient;
        PubSubClient mqttClient;
        std::map<std::string, std::vector<std::function<void(char*, uint8_t*, unsigned int)>>> callbacks;
};

#endif
