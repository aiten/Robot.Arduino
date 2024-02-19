#include <arduino.h>
#include <cstdint>

#include "Motor.h"

double Kp = 0.025, Ki = 0.45, Kd = 0.001;
//double Kp = 0.1, Ki = 0.25, Kd = 0.0;

const int maxTime = 100;
const int minTime = 10;

const int scaleTo = 0;
const int scaleFrom = 1023;

Motor* Motor::_allMotors[4] = {0, 0, 0, 0};

Motor::Motor(uint8_t fwPin, uint8_t bwPin, uint8_t pulsePin) :
	_fwPin(fwPin), _bwPin(bwPin), _pulsePin(pulsePin), _myPID(&_input, &_output, &_setpoint, Kp, Ki, Kd)
{
	_currentPin = 0xff;
	_isBusy = false;
	_myPID.SetSampleTime(20);
}

void Motor::Setup()
{
	Stop();

	pinMode(_pulsePin, INPUT);

	if (_allMotors[0] == 0)
	{
		attachInterrupt(digitalPinToInterrupt(_pulsePin), StaticDecoderInterrupt0, RISING);
		_motorId = 0;
	}
	else if (_allMotors[1] == 0)
	{
		attachInterrupt(digitalPinToInterrupt(_pulsePin), StaticDecoderInterrupt1, RISING);
		_motorId = 1;
	}

	_allMotors[_motorId] = this;

	_setpoint = 0.0;
	_input = 0.0;

	_lastDecoderTime = millis();
}

bool Motor::IsBusy()
{
	return _isBusy;
}

void Motor::Poll()
{
	if (_isBusy)
	{
		uint32_t now = millis();

		if (_untilTime < now)
		{
			Stop();
		}
		else
		{
			if (_lastDecoderTime + maxTime < now)
			{
				// motor stoped
				_decoderTime = maxTime;
			}

			_input = map(min(max((int) _decoderTime,minTime), maxTime), minTime, maxTime, scaleFrom, scaleTo);

			if (_myPID.Compute())
			{
				int output = (int) _output;

				if (_oldOutput != output)
				{
					_oldOutput = output;
					analogWrite(_currentPin, output);
				}
				Serial.printf("%d i=%3d s=%3d o=%3d d=%3d\n", (int)_motorId, (int)_input, (int)_setpoint, (int)_output,
				              _decoderTime);

#if defined(_MSC_VER)
				static int lastvalues[4] = {0};
				static int lastIdx = 0;
				lastvalues[lastIdx] = (long)_output;

				lastIdx = (lastIdx + 1) % 4;

				_decoderTime = map(lastvalues[lastIdx], 0, 255, 100, 10);
				_lastDecoderTime = millis() + 1000000;
#endif
			}
		}
	}
}

void Motor::Start(int speed)
{
	GoTo(speed, 1000000, 10000000);
}

void Motor::GoTo(int speed, int distance, int duration)
{
	if (speed == 0)
	{
		Stop();
	}
	else
	{
		uint32_t now = millis();
		uint8_t pwmPin = speed > 0 ? _fwPin : _bwPin;
		bool samePin = _currentPin == pwmPin;

		_distance = distance;
		_isBusy = true;
		_setpoint = abs(speed);

		if (_myPID.GetMode() == PID::AUTOMATIC && samePin)
		{
			// we are running (in the same direction), but different speed
		}
		else
		{
			// start
			_currentPin = pwmPin;
			_lastDecoderTime = now;
			_decoderTime = 100;
			_oldOutput = -1;
			analogWrite(_fwPin, 0);
			analogWrite(_bwPin, 0);
			_myPID.Start(min(abs(speed) / 4, 100));
		}
	
		_untilTime = now + duration;
	}
}

void Motor::Stop()
{
	_myPID.Stop();
	_isBusy = false;
	analogWrite(_fwPin, 0);
	analogWrite(_bwPin, 0);
}

void Motor::DecoderInterrupt()
{
	uint32_t time = millis();
	_decoderTime = time - _lastDecoderTime;
	_lastDecoderTime = time;
}

IRAM_ATTR void Motor::StaticDecoderInterrupt0()
{
	_allMotors[0]->DecoderInterrupt();
}

IRAM_ATTR void Motor::StaticDecoderInterrupt1()
{
	_allMotors[1]->DecoderInterrupt();
}
