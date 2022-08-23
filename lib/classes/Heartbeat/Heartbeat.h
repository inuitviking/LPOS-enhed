#ifndef LPOS_ENHED_HEARTBEAT_H
#define LPOS_ENHED_HEARTBEAT_H


class Heartbeat {
protected:
	// Properties
	bool peaked = false;
	bool caved = true;
	int peakMinimum = 850;
	int peakCount = 0;
	int recordTime = 5;
	int bpm = 0;
	int ledState = LOW;
	unsigned long lastMillis = millis();

public:
	/**
	 * GetBPM gets the heart rate based off of analog input.
	 * This method has been written in conjunction the use of DFRobot SEN0203 (https://wiki.dfrobot.com/Heart_Rate_Sensor_SKU__SEN0203).
	 * The function takes any analog input, as it does not use the sensors own library, as it seemed faulty and unresponsive, in either digital or analog mode.
	 * It looks for peaks in the analog output (set to 850 for this project), records beats in five seconds.
	 * This means it updates the BPM every five seconds, which was the minimum required to get a more accurate reading.
	 *
	 * @param pinNumber - The analog pin to listen on
	 * @param ledPin - The LED pin to show beats visually.
	 * @return int: BPM as int measured over the past 5 seconds
	 */
	int GetBPM(uint8_t pinNumber, uint8_t ledPin);
};


#endif //LPOS_ENHED_HEARTBEAT_H
