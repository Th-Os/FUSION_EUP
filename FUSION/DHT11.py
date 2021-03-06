from .FUSION_MQTT import *

class DHT11(FUSION_MQTT):
    def __init__(self, node_name, node_network=MQTT_DEFAULT_NETWORK, node_location=MQTT_DEFAULT_LOCATION):
        FUSION_MQTT.__init__(self, node_name, node_network, node_location)
        self.add_data_entry("temperature", int)
        self.add_data_entry("humidity", int)

    def info(self):
        print("DHT11 Digital Humidity and Temperature Sensor")
