#pragma once

class PID
{
public:
	enum EMode
	{
		AUTOMATIC = 1,
		MANUAL = 0
	};

	enum EDirection
	{
		DIRECT = 0,
		REVERSE = 1
	};

	enum EProportionalMode
	{
		P_ON_M = 0,
		P_ON_E = 1
	};

	PID(double*, double*, double*, double kp, double ki, double kd, EProportionalMode = P_ON_E, EDirection = DIRECT);

	void Start(double startWithOutput);
	void Stop();

	bool Compute();

	void SetOutputLimits(double minOut, double maxOut);
	void SetTunings(double kp, double ki, double kd, EProportionalMode, EDirection);
	void SetSampleTime(uint32_t sampleTime);

	double GetKp() const { return _dispKp; }
	double GetKi() const { return _dispKi; }
	double GetKd() const { return _dispKd; }

	EMode GetMode() const { return _mode; }
	EDirection GetDirection() const { return _controllerDirection; }

	double GetTi() const { return GetKp() / GetKi(); }
	double GetTd() const { return GetKd() / GetKp(); };

private:
	double* _myInput;
	double* _myOutput;
	double* _mySetPoint;

	double _outputSum; // * internal integrator state for understanding and user-space control
	double _lastInput;

	double _dispKp; // * we'll hold on to the tuning parameters in user-entered 
	double _dispKi; //   format for display purposes
	double _dispKd; //

	double _kp; // * (P)roportional Tuning Parameter
	double _ki; // * (I)ntegral Tuning Parameter
	double _kd; // * (D)erivative Tuning Parameter

	uint32_t _lastTime;
	uint32_t _sampleTime;

	double _outMin;
	double _outMax;

	EMode _mode;
	EProportionalMode _pOnE;
	EDirection _controllerDirection;

	void LimitOutput(double* output);
};
