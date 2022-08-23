//
// Created by angus on 8/23/22.
//

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

	static PubSubClient Reconnect(PubSubClient mqttClient);
};


#endif //LPOS_ENHED_MQTT_H
