#include "Arduino.h"
#include <MyPID.h>

PID::PID(double* input, double* output, double* setpoint,
         double kp, double ki, double kd,
         EProportionalMode pMode, EDirection direction)
{
	_myOutput = output;
	_myInput = input;
	_mySetPoint = setpoint;

	_mode = MANUAL;
	_sampleTime = 100;
	*_myOutput = 0.0;

	SetOutputLimits(0, 255);
	SetTunings(kp, ki, kd, pMode, direction);

	_lastTime = millis() - _sampleTime;
}


bool PID::Compute()
{
	if (_mode != AUTOMATIC) return false;

	uint32_t now = millis();
	uint32_t timeChange = (now - _lastTime);

	if (timeChange < _sampleTime) return false;

	double input = *_myInput;
	double error = *_mySetPoint - input;
	double dInput = input - _lastInput;

	_outputSum += _ki * error;

	if (_pOnE == P_ON_M) _outputSum -= _kp * dInput;

	LimitOutput(&_outputSum);

	double output =
		(_pOnE == P_ON_E ? _kp * error : 0.0) +
		(_outputSum - _kd * dInput);

	if (output > _outMax)
	{
		_outputSum -= output - _outMax; // backcalculate integral to feasability
		output = _outMax;
	}
	else if (output < _outMin)
	{
		_outputSum += _outMin - output; // backcalculate integral to feasability
		output = _outMin;
	}

	*_myOutput = output;

	_lastInput = input;
	_lastTime = now;

	return true;
}

void PID::SetTunings(double kp, double ki, double kd, EProportionalMode pMode, EDirection direction)
{
	if (kp < 0 || ki < 0 || kd < 0) return;

	_pOnE = pMode;
	_controllerDirection = direction;

	_dispKp = kp;
	_dispKi = ki;
	_dispKd = kd;

	double sampleTimeInSec = _sampleTime / 1000.0;

	_kp = kp;
	_ki = ki * sampleTimeInSec;
	_kd = kd / sampleTimeInSec;

	if (_controllerDirection == REVERSE)
	{
		_kp = -_kp;
		_ki = -_ki;
		_kd = -_kd;
	}
}

void PID::SetSampleTime(uint32_t sampleTime)
{
	if (sampleTime > 0)
	{
		double ratio = (double)sampleTime / (double)_sampleTime;
		_ki *= ratio;
		_kd /= ratio;
		_sampleTime = sampleTime;
	}
}

void PID::SetOutputLimits(double minOut, double maxOut)
{
	if (minOut >= maxOut) return;
	_outMin = minOut;
	_outMax = maxOut;

	if (_mode == AUTOMATIC)
	{
		LimitOutput(_myOutput);
		LimitOutput(&_outputSum);
	}
}

void PID::Start(double startWithOutput)
{
	*_myOutput = startWithOutput;

	LimitOutput(_myOutput);

	_outputSum = *_myOutput;
	_lastInput = *_myInput;
	_mode = AUTOMATIC;
}

void PID::Stop()
{
	_mode = MANUAL;
}

void PID::LimitOutput(double* output)
{
	if (*output > _outMax) *output = _outMax;
	else if (*output < _outMin) *output = _outMin;
}
