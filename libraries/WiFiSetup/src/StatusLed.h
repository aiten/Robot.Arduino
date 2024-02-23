#pragma once

////////////////////////////////////////////////////////

#include "EepromConfig.h"

////////////////////////////////////////////////////////

class StatusLed
{
private:
	uint8_t _statusPin;
	bool _isLedOn = false;
	bool _inToggle = false;

	uint32_t _until = 0;
	uint32_t _onOffTime;

public:
	StatusLed(uint8_t statusPin, uint32_t onOffTime) : _statusPin(statusPin), _onOffTime(onOffTime)
	{
		if (HasStatusPin())
		{
			pinMode(_statusPin, OUTPUT);
		}
	}

public:
	void Loop()
	{
		if (HasStatusPin() && _until < millis())
		{
			SetOnOff(!_isLedOn, _onOffTime);
			_inToggle = false;
		}
	}
	void ToggleNow(uint32_t time)
	{
		if (!_inToggle && HasStatusPin())
		{
			SetOnOff(!_isLedOn, time);
			_inToggle = true;
		}
	}

	void SetOnOffTime(uint32_t time)
	{
		_onOffTime = time;
	}

	void SetLed(bool setOn, uint32_t time)
	{
		if (HasStatusPin())
		{
			SetOnOff(setOn, time);
		}
	}


private:

	bool HasStatusPin() const { return _statusPin != 0xff; }

	void SetOnOff(bool ledOn, uint32_t time)
	{
		_isLedOn = ledOn;
		_until = millis() + time;
		digitalWrite(_statusPin, !_isLedOn);
	}
};
