#pragma once

#include <MyPID.h>

//////////////////////////////////////////

class Motor
{
public:
	Motor(uint8_t fwPin, uint8_t bwPin, uint8_t pulsePin);

	void Setup();
	void Poll();

	void GoTo(int speed, int distance, int duration);
	void Start(int speed);

	void Stop();

	bool IsBusy();

private:
	const uint8_t _fwPin;
	const uint8_t _bwPin;
	const uint8_t _pulsePin;
	uint8_t _motorId;

	uint32_t _untilTime;
	uint32_t _untilTimeOutput;

	uint8_t _currentPin;
	int32_t _oldOutput;

	int _distance;

	bool _isBusy;
	PID _myPID;

	double _setpoint;
	double _input;
	double _output;

	volatile uint32_t _lastDecoderTime;
	volatile uint32_t _decoderTime;
	static void StaticDecoderInterrupt0();
	static void StaticDecoderInterrupt1();
	void DecoderInterrupt();

	static Motor* _allMotors[4];
};
