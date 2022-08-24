#include <Arduino.h>
#include <PubSubClient.h>
#include <protothreads.h>
#include "MQTT.h"
#include "../../include/Secrets/mqtt.h"
#include "../../include/definitions.h"

void MQTT::CallBack(char *topic, byte *payload, unsigned int length) {
	// Say on which topic we received a message.
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	// Print payload.
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();
}

PubSubClient MQTT::Reconnect(PubSubClient mqttClient) {
	// Loop until we're reconnected
	while (!mqttClient.connected()) {
		Serial.print("Reattempting MQTT connection...");
		// Attempt to connect
		if (mqttClient.connect(HOSTNAME, MQTT_ACCT, MQTT_PASS)) {
			Serial.println("connected");
			// Once connected, publish an announcement...
			mqttClient.publish("hospital/", HOSTNAME);
			// ... and resubscribe
			mqttClient.subscribe("hospital/");
		} else {
			Serial.print("failed, rc=");
			Serial.print(mqttClient.state());
			Serial.println(" try again in 5 seconds");
		}
	}

	return mqttClient;
}
