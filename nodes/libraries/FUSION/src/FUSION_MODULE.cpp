#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "FUSION_MODULE.h"

bool mqtt_initialized = false;
FusionMQTT *mqtt;

// initialize builtin reset function
void(* resetFunc) (void) = 0;

FusionModule::FusionModule()
{
    nodeId = node_id;
}

// initialize the the correct communication protocol
void FusionModule::initialize()
{
    if(protocol == PROTOCOL_WIFI)
    {
        Serial.println("WIFI MODE");
        if(initWifi() == true) Serial.println("wifi initialized");
    }

    if(protocol == PROTOCOL_MQTT)
    {
        Serial.println("MQTT MODE");
        if(!mqtt_initialized)
        {
            mqtt = new FusionMQTT();
            mqtt->init();
            mqtt_initialized = true;
        }
    }
}

// you might want to override this function
// everything that should happen each frame belongs here
void FusionModule::update()
{
    if(protocol == PROTOCOL_MQTT)
    {
        mqtt->update(); // make sure messages are sent and received
    }
}

void FusionModule::createPacket(char* data, int data_length)
{
    createPacket(data, data_length, MSG_TYPE_PACKET);
}

// build a packet with header, data and checksum to be sent bytewise
// needed for lightweight protocols like UDP
void FusionModule::createPacket(char* data, int data_length, int type)
{
	packetLength = data_length + 7;
	int index_checksum = 1 + FRAME_HEAD_LENGTH + data_length;
	packet = (char*) malloc(2 + FRAME_HEAD_LENGTH + data_length + FRAME_CHECKSUM_LENGTH);
	
	packet[INDEX_FRAME_BEGIN] = FRAME_BEGIN;
	packet[INDEX_MSG_TYPE] = type;
	packet[INDEX_MSG_ID] = 0;
	packet[INDEX_NI] = nodeId;
	packet[INDEX_NMB_DATA] = data_length;
    
    for(int i = 0; i < data_length; i++)
    {
        packet[1 + FRAME_HEAD_LENGTH + i] = data[i];
    }

	packet[index_checksum] = 1;
	packet[index_checksum + 1] = 1;
	packet[index_checksum + 2] = 0; // why?
}

void FusionModule::freePacket()
{
    free(packet);
}

// Test method for sending a retain message

void FusionModule::sendDataRetain(char* data, char* topic_data)
{
    mqtt->send(data, topic_data, true);
}

// wrapper functions for sending data without caring about mqtt topics

void FusionModule::sendData(bool data)
{
    sendData(data, TOPIC_UNDEFINED);
}

void FusionModule::sendData(int data)
{
    sendData(data, TOPIC_UNDEFINED);
}

void FusionModule::sendData(uint8_t data)
{
    sendData(data, TOPIC_UNDEFINED);
}

void FusionModule::sendData(uint16_t data)
{
    sendData(data, TOPIC_UNDEFINED);
}

void FusionModule::sendData(char data)
{
    sendData(data, TOPIC_UNDEFINED);
}

void FusionModule::sendData(char* data, unsigned int length)
{
    sendData(data, TOPIC_UNDEFINED);
}

// send data with selected protocol
void FusionModule::sendData(char* data, int data_length, char* topic_data)
{
    if(protocol == PROTOCOL_WIFI)
    {
        createPacket(data, data_length);
        sendPacket(packet, packetLength);
        freePacket();
    }

    if(protocol == PROTOCOL_MQTT)
    {
        mqtt->send(data, data_length, topic_data);
    }
}

// wrapper functions for sending data with a mqtt topic

void FusionModule::sendData(char data, char* topic_data)
{
    char data_pack[1] = {data};
    sendData(data_pack, 1, topic_data);
}

void FusionModule::sendData(uint8_t data, char* topic_data)
{
    char data_pack[1] = {(char) data};
    sendData(data_pack, 1, topic_data);
}

void FusionModule::sendData(int data, char* topic_data)
{
    unsigned int length = sizeof(data);
    char data_pack[length];
    for(int i = 0; i < length; i++)
    {
        data_pack[i] = (char) (data >> (i * sizeof(char) * 8));
    }
    sendData(data_pack, length, topic_data);
}

void FusionModule::sendData(uint16_t data, char* topic_data)
{
    unsigned int length = sizeof(uint16_t);
    char data_pack[length];
    for(int i = 0; i < length; i++)
    {
        data_pack[i] = (char) (data >> (i * sizeof(char) * 8));
    }
    sendData(data_pack, length, topic_data);
}

void FusionModule::sendData(bool data, char* topic_data)
{
    unsigned int length = sizeof(data);
    char data_pack[length];
    for(int i = 0; i < length; i++)
    {
        data_pack[i] = (char) (data >> (i * sizeof(char) * 8));
    }
    sendData(data_pack, length, topic_data);
    Serial.println("Sent false data.");
}
