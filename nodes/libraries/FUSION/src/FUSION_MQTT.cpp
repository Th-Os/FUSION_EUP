#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include "FUSION_MQTT.h"

// initialize variables from compiler params
FusionMQTT::FusionMQTT()
{
    topic_network = STR(MQTT_TOPIC_NETWORK); //"FUSION";
    topic_location = STR(MQTT_TOPIC_LOCATION); //"1104";
    topic_name = STR(NODE_NAME);
}

// set up the wifi and mqtt connection
void FusionMQTT::init()
{
    // TODO do only once!
    mqttClient = PubSubClient(wifiClient);
    WiFi.begin(wifi_ssid, wifi_password);

    // connect to wifi
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
    }

    mqttClient.setServer(mqtt_server, mqtt_port);

    // registers a member function of this instance
    // as a callback for mqtt messages
    // the strange syntax is necessary because of
    // the weird way C++ handles pointers to member functions
    mqttClient.setCallback(std::bind(&FusionMQTT::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // member variable maybe?
    char topic[TOPIC_MAXLENGTH];
    snprintf(topic, TOPIC_MAXLENGTH, "%s/%s/%s/#", topic_network, topic_location, topic_name);

    char will[1000];
    snprintf(will, 1000, "%s disconnected", topic_name);

    while (!mqttClient.connected())
    {
        if (!mqttClient.connect(topic_name, topic, 0, 0, will))
        {
            delay(100);
        }
    }

    // listen to all messages addressed with topic
    mqttClient.subscribe(topic);
    Serial.println("initialized");
}

// wrapper functions for mqtt.publish()
// overloaded to match all relevant data types

void FusionMQTT::send(uint16_t data, char* topic_data)
{
    uint8_t *bytes = (uint8_t*) malloc(2);
    bytes[0] = data >> 8;
    bytes[1] = data & 0xFF;
    send(bytes, 2, topic_data);
}

void FusionMQTT::send(char* data, unsigned int length, char* topic_data)
{
    char topic[TOPIC_MAXLENGTH];
    snprintf(topic, TOPIC_MAXLENGTH, "%s/%s/%s/%s", topic_network, topic_location, topic_name, topic_data);
    mqttClient.publish(topic, (uint8_t*) data, length, false);
    mqttClient.loop();
    Serial.println(topic);
}

void FusionMQTT::send(uint8_t* data, unsigned int length, char* topic_data)
{
    char topic[TOPIC_MAXLENGTH];
    snprintf(topic, TOPIC_MAXLENGTH, "%s/%s/%s/%s", topic_network, topic_location, topic_name, topic_data);
    mqttClient.publish(topic, data, length, false);
    mqttClient.loop();
    Serial.println(topic);
}

void FusionMQTT::send(const char* data, char* topic_data)
{
    char topic[TOPIC_MAXLENGTH];
    snprintf(topic, TOPIC_MAXLENGTH, "%s/%s/%s/%s", topic_network, topic_location, topic_name, topic_data);
    mqttClient.publish(topic, data, false);
    mqttClient.loop();
}

void FusionMQTT::send(const char* data, char* topic_data, bool retain)
{
    char topic[TOPIC_MAXLENGTH];
    snprintf(topic, TOPIC_MAXLENGTH, "%s/%s/%s/%s", topic_network, topic_location, topic_name, topic_data);
    mqttClient.publish(topic, data, retain);
    mqttClient.loop();
}

// call this to make sure the mqtt module sends and receives messages
void FusionMQTT::update()
{
    mqttClient.loop();
}

// this functions is called when a message is received over mqtt
// it iterates over all registered callbacks and calls the ones
// matching the topic of the mqtt message
void FusionMQTT::callback(char* topic, uint8_t* payload, unsigned int length)
{
    for(std::function<void(char*, uint8_t*, unsigned int)> cb : callbacks[topic])
    {
        cb(topic, payload, length);
    }
}

// registers a function as a callback for given mqtt topic
// the function pointer is stored in a hashmap under the given topic as a key
void FusionMQTT::registerCallback(std::function<void(char*, uint8_t*, unsigned int)> callback_function, char* topic_data)
{
    char topic[TOPIC_MAXLENGTH];
    snprintf(topic, TOPIC_MAXLENGTH, "%s/%s/%s/%s", topic_network, topic_location, topic_name, topic_data);

    callbacks[topic].push_back(callback_function);
}
