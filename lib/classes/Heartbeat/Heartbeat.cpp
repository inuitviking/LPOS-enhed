#include <Arduino.h>
#include "Heartbeat.h"
#include "../../include/definitions.h"

int Heartbeat::GetBPM (uint8_t pinNumber, uint8_t ledPin) {
	int sensorValue = analogRead(pinNumber);
	if (sensorValue >= peakMinimum) {
		if (caved) {
			peaked = true;
			caved = false;
			ledState = HIGH;
			peakCount++;
		}
	} else if (sensorValue < peakMinimum) {
		if (peaked) {
			peaked = false;
			caved = true;
			ledState = LOW;
		}
	}
	if ((millis() - lastMillis) >= (recordTime * 1000)) {
		bpm = 60*(peakCount)/recordTime;
		peakCount = 0;
		lastMillis = millis();
	}
	digitalWrite(ledPin, ledState);
	return bpm;
}