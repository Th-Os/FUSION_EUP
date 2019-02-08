#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "FUSION_PIN.h"

std::vector<FusionPin*> FusionPin::interruptPins_change;
std::vector<FusionPin*> FusionPin::interruptPins_rise;
std::vector<FusionPin*> FusionPin::interruptPins_fall;

FusionPin::FusionPin(unsigned int pin_id) : FusionModule()
{
    pin = pin_id;
    isAnalog = (pin == A0);
    directionSet = false;
}

void FusionPin::initialize()
{
    FusionModule::initialize();
    
    snprintf(topic_pin, 2, "%d", pin);

    registerCallbacks();
}

void FusionPin::initialize(bool dir)
{
    setDirection(dir);
    initialize();
}

void FusionPin::update()
{
    FusionModule::update();
    if(streamOn) streamData();
}

void FusionPin::registerCallbacks()
{
    char* commands[] = {"digitalRead", "digitalWrite", "analogRead", "analogWrite", "setDirection", "setInterrupt", "streamData"};

    for(char* command : commands)
    {
        // TODO test this!
        mqtt.registerCallback(std::bind(&FusionPin::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), command);
    }
}

void FusionPin::mqttCallback(char* topic, byte* payload, int length)
{
    uint16_t data;

    if(length == 1) data = payload[0] - '0';
    else if(length == 2) data = ((payload[1] - '0') << 8) | (payload[0] - '0');

    if(strstr(topic, "digitalRead")) dRead();
    else if(strstr(topic, "digitalWrite")) dWrite((bool) data);
    else if(strstr(topic, "analogRead")) aRead();
    else if(strstr(topic, "analogWrite")) aWrite(data);
    else if(strstr(topic, "setDirection")) setDirection((bool) data);
    else if(strstr(topic, "setInterrupt")) setInterrupt((unsigned int) data);
    else if(strstr(topic, "streamData"))
    {
        streamOn = true;
        streamDelay = data;
        streamTimer = millis();
    }
}

void FusionPin::dWrite(bool value)
{
    digitalWrite(pin, value);
}

bool FusionPin::dRead()
{
    bool data = digitalRead(pin);
    sendData(data, "digitalReadResult");
    return data;
}

void FusionPin::aWrite(uint16_t value)
{
    analogWrite(pin, value);
}

uint16_t FusionPin::aRead()
{
    uint16_t data = analogRead(pin);
    sendData(data, "analogReadResult");
    return data;
}

void FusionPin::streamData()
{
    long time = millis();
    if(time - streamTimer >= streamDelay)
    {
        streamTimer = time;
        isAnalog ? aRead() : dRead();
    }
}

void FusionPin::setDirection(bool dir)
{
    pinMode(pin, dir);

    direction = dir;
    directionSet = true;
}

void FusionPin::setInterrupt(unsigned int edge)
{
    pinMode(pin, INPUT_PULLUP);

    switch(edge)
    {
        case CHANGE:
            attachInterrupt(digitalPinToInterrupt(pin), interruptHandler_change, edge);
            interruptPins_change.push_back(this);
            break;
        case RISING:
            attachInterrupt(digitalPinToInterrupt(pin), interruptHandler_rise, edge);
            interruptPins_rise.push_back(this);
            break;
        case FALLING:
            attachInterrupt(digitalPinToInterrupt(pin), interruptHandler_fall, edge);
            interruptPins_fall.push_back(this);
            break;
    }

    directionSet = true;
    direction = INPUT;
}

void FusionPin::onChange()
{
    sendData(topic_pin, 2, "change");
}

void FusionPin::onRise()
{
    sendData(topic_pin, 2, "rise");
}

void FusionPin::onFall()
{
    sendData(topic_pin, 2, "fall");
}

void FusionPin::interruptHandler_change()
{
    for(FusionPin* p : interruptPins_change)
    {
        p->onChange();
    }
}

void FusionPin::interruptHandler_rise()
{
    for(FusionPin* p : interruptPins_rise)
    {
        p->onRise();
    }
}

void FusionPin::interruptHandler_fall()
{
    for(FusionPin* p : interruptPins_fall)
    {
        p->onFall();
    }
}
