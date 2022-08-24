#ifndef LPOS_ENHED_MQTT_H
#define LPOS_ENHED_MQTT_H


class MQTT {
public:
	/**
	 * This prints a message to the Serial monitor whenever we receive a message over MQTT.
	 * @param topic
	 * @param payload
	 * @param length
	 */
	static void CallBack(char* topic, byte* payload, unsigned int length);

	/**
	 *This tries to reconnect to the MQTT broker.
	 * @param mqttClient
	 * @return PubSubClient - The same PubSubClient that is in the argument is returned with all necessary updates.
	 */
	static PubSubClient Reconnect(PubSubClient mqttClient);
};


#endif //LPOS_ENHED_MQTT_H
