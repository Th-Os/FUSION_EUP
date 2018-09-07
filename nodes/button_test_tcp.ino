#define DEBUG 1
#include "./src/FUSION/FUSION_WIFI.h"
#include "./src/FUSION/FUSION_MODULE.h"

bool wasDown = false;

uint8_t buttonPin = D3;

FusionModule module(43);

void setup()
{
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  if(initWifi() == true) Serial.println("connected!");
}

void loop()
{
  bool buttonDown = digitalRead(buttonPin);
  if(buttonDown == LOW)
  {
    if(wasDown == false)
    {
      wasDown = true;
      Serial.println("pressed");
      sendButtonEvent(true);
    }
  }
  else
  {
    if(wasDown == true)
    {
      wasDown = false;
      Serial.println("released");
      sendButtonEvent(false);
    }
  }

  delay(50);
}

void sendButtonEvent(bool entered)
{
  //char* packet;
  char data[1] = {(char) entered};
  /*unsigned int packet_length = createPacket(data, 1, packet);
  for(int i = 0; i < packet_length; i++)
  {
    Serial.println(packet[i], HEX);
  }*/
  module.createPacket(data, 1);
  sendPacket(module.packet, module.packetLength);
  module.freePacket();
}
