#!/bin/bash
. ./fusion.conf

for ARGUMENT in "$@"
do
    KEY=$(echo $ARGUMENT | cut -f1 -d=)
    VALUE=$(echo $ARGUMENT | cut -f2 -d=)   

    case "$KEY" in
            NODE_NAME) NODE_NAME=${VALUE} ;;
            NODE_ID)   NODE_ID=${VALUE} ;;     
            PIN)       PIN=${VALUE} ;;     
            DELAY)     DELAY=${VALUE} ;;
            PROTOCOL)  PROTOCOL=${VALUE} ;;
            ACTION)    ACTION=${VALUE} ;;
            *)   
    esac    
done

mkdir nodes/pio/src
mkdir nodes/pio/lib
cp -r nodes/$1/* nodes/pio/src/
cp -r nodes/libraries/FUSION/* nodes/pio/lib
cd nodes/pio

FLAGS=""

if [ ! -z "$NODE_NAME" ]; then
    FLAGS="$FLAGS -DNODE_NAME=\"$NODE_NAME\""
fi
    
if [ ! -z "$NODE_ID" ]; then
    FLAGS="$FLAGS -DNODE_ID=$NODE_ID"
fi
    
if [ ! -z "$PIN" ]; then
    FLAGS="$FLAGS -DPIN=$PIN"
fi
    
if [ ! -z "$WIFI_SSID" ]; then
    FLAGS="$FLAGS -DWIFI_SSID=\"$WIFI_SSID\""
fi
    
if [ ! -z "$WIFI_PASSWORD" ]; then
    FLAGS="$FLAGS -DWIFI_PASSWORD=\"$WIFI_PASSWORD\""
fi
    
if [ ! -z "$MQTT_SERVER" ]; then
    FLAGS="$FLAGS -DMQTT_SERVER=\"$MQTT_SERVER\""
fi
    
if [ ! -z "$MQTT_PORT" ]; then
    FLAGS="$FLAGS -DMQTT_PORT=$MQTT_PORT"
fi
    
if [ ! -z "$MQTT_TOPIC_NETWORK" ]; then
    FLAGS="$FLAGS -DMQTT_TOPIC_NETWORK=\"$MQTT_TOPIC_NETWORK\""
fi
    
if [ ! -z "$MQTT_TOPIC_LOCATION" ]; then
    FLAGS="$FLAGS -DMQTT_TOPIC_LOCATION=\"$MQTT_TOPIC_LOCATION\""
fi
    
if [ ! -z "$DELAY" ]; then
    FLAGS="$FLAGS -DDELAY=$DELAY"
fi
    
if [ ! -z "$PROTOCOL" ]; then
    FLAGS="$FLAGS -DPROTOCOL=\"$PROTOCOL\""
fi
    
export PLATFORMIO_BUILD_FLAGS="$FLAGS"

#export PLATFORMIO_BUILD_FLAGS="-DNODE_NAME=\"$NODE_NAME\" -DNODE_ID=$NODE_ID -DPIN=$PIN -DWIFI_SSID=\"$WIFI_SSID\" -DWIFI_PASSWORD=\"$WIFI_PASSWORD\" -DMQTT_SERVER=\"$MQTT_SERVER\" -DMQTT_PORT=$MQTT_PORT -DMQTT_TOPIC_NETWORK=\"$MQTT_TOPIC_NETWORK\" -DMQTT_TOPIC_LOCATION=\"$MQTT_TOPIC_LOCATION\" -DDELAY=$DELAY -DPROTOCOL=\"$PROTOCOL\""
echo "$PLATFORMIO_BUILD_FLAGS"
if [ $ACTION = "debug" ]; then
    echo "debug..."
    platformio run --target debug
else
    platformio run --target upload
fi

rm -r src/*
cd ../..

