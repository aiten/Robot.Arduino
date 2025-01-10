#pragma once

#include "MqttClient.h"
#include "PushButton.h"

class Plate
{

public:
  Plate(MqttClient &mqttClient, StatusLed &statusLed) : _mqttClient(mqttClient), _statusLed(statusLed)
  {
  }

private:
  MqttClient &_mqttClient;
  StatusLed &_statusLed;

  const long SEND_INTERVAL = 150;
  const long SEND_INTERVAL_ADD = 150; // add time to go
  const int MIN_SPEED = 60;
  const int MAX_SPEED = 255;

  void LimitPublishGo(uint direction, uint speed, uint duration)
  {
    static bool _speed0Sent = false;
    if (speed < MIN_SPEED)
    {
      speed = 0;
    }
    else if (speed > 255)
    {
      speed = 255;
    }

    if (speed != 0 || _speed0Sent == false)
    {
      _mqttClient.PublishGo(direction, speed, duration);
      _speed0Sent = speed == 0;
    }
  }

public:
  void setupPlate()
  {
    pinMode(LEFT_PIN,INPUT_PULLUP);
    pinMode(RIGHT_PIN,INPUT_PULLUP);
  }

  enum EState
  {
    WaitForLeftOn = 0,
    WaitForLeftOff,
    WaitForRightOn,
    WaitForRightOff,
    LastState
  };

  EState currentState = WaitForLeftOn;
  EState lastState = WaitForRightOff;
  long stateSinceTime = millis();

  #define LASTTIMECOUNT 6
  long last_stepTimes[LASTTIMECOUNT] = {0}; // Array to store the last 5 ...
  int stepTimeIdx = 0;          // Circular index for the array

  #define LASTPINTIMECOUNT 6
 
  long last_pinRTimes[LASTPINTIMECOUNT] = {0}; // Array to store the last x ...
  int pinRTimeIdx = 0;          // Circular index for the array

  long last_pinLTimes[LASTPINTIMECOUNT] = {0}; // Array to store the last x ...
  int pinLTimeIdx = 0;          // Circular index for the array

  void calcPinCount()
  {
    static int rPinCount = 0;
    static int lPinCount = 0;

    static int lPinOld = -1;
    static int rPinOld = -1;

    long now = millis();

    int lPin = digitalRead(LEFT_PIN);
    int rPin = digitalRead(RIGHT_PIN);

    if (lPinOld != lPin)
    {
      if (lPin==HIGH) // released
      {
        static long timePinLChanged = 0;
        long timeForLChanged = now - timePinLChanged;

        if (timeForLChanged > PRELLEN_TIME)
        {
          timePinLChanged = now;
          lPinCount++;
          last_pinLTimes[pinLTimeIdx] = timePinLChanged;    // store abs times and not diff
          pinLTimeIdx = (pinLTimeIdx + 1) % LASTPINTIMECOUNT;
        }
      }
      lPinOld = lPin;
    }

    if (rPinOld != rPin)
    {
      if (rPin==HIGH) // released
      {
        static long timePinRChanged = 0;
        long timeForRChanged = now - timePinRChanged;

        if (timeForRChanged > PRELLEN_TIME)
        {
          timePinRChanged = now;
          rPinCount++;
          last_pinRTimes[pinRTimeIdx] = timePinRChanged;    // store abs times and not diff
          pinRTimeIdx = (pinRTimeIdx + 1) % LASTPINTIMECOUNT;
        }
      }
      rPinOld = rPin;
    }
  }

  int calcSpeed()
  {
    calcPinCount();

    static int lastSpeed = 0;

    long now = millis();

    static long timeLastCalc = 0;
    const int MinTimeCall = 2; // 2 ms

    if (now - timeLastCalc  <= MinTimeCall)
    {
      return lastSpeed;
    }
    timeLastCalc = now;

    static int pinForState[] = {LEFT_PIN, RIGHT_PIN, RIGHT_PIN, LEFT_PIN};
    static int waitForState[] = {LOW, HIGH, LOW, HIGH};

    if (digitalRead(pinForState[currentState]) == waitForState[currentState])
    {
      currentState = (EState)((currentState + 1) % LastState);
      long timeInState = now - stateSinceTime;
      stateSinceTime = now;

      if (currentState == WaitForRightOff || currentState == WaitForLeftOff)
      {
        static long stepSinceTime = 0;
        long timeStep = now - stepSinceTime;
        stepSinceTime = now;

        last_stepTimes[stepTimeIdx] = timeStep;
        stepTimeIdx = (stepTimeIdx + 1) % LASTTIMECOUNT;

        long totalTime=0;

        for (int i = 0; i < LASTTIMECOUNT; ++i) 
        {
          totalTime += last_stepTimes[i];
          if (last_stepTimes[i] == 0)
          {
            totalTime += MAXTIME;
          }
        }

        long avgTime = totalTime / LASTTIMECOUNT;

        if (avgTime > MAXTIME)
        {
          avgTime = MAXTIME;
        }
        else if (avgTime < MINTIME)
        {
          avgTime = MINTIME;
        }

        lastSpeed = map((int)avgTime,MAXTIME,MINTIME,MIN_SPEED,MAX_SPEED);
      }
    }
    else
    {
      if (now - stateSinceTime > MAXTIME)
      {
        // NoChange();
        lastSpeed = 0;

        for (int i = 0;i<LASTTIMECOUNT;i++)
        {
          last_stepTimes[i] = 0;
        }
      }
    }

    return lastSpeed;
  }


  void loopPlate()
  {

    int direction=0;
    int speed = calcSpeed();

    static long until = 0;
    static bool isOn = PUSHBUTTON_ISON;
    static CPushButton pushButton(PUSHBUTTON_PIN, LOW);

    if (pushButton.IsOn())
    {
      // toggle state
      isOn = !isOn;
      // statusLed.SetOnOffTime(isOn ? 1000: 100);
      _statusLed.SetLed(isOn, 100000000);
    }

    long now = millis();

    if (now < until || !isOn)
    {
      return;
    }

    until = now + SEND_INTERVAL;

    // check for move left/right
    if (speed == 0)
    {
      long timeTurnL = now - last_pinLTimes[(pinLTimeIdx - HITCOUNTTURN + LASTPINTIMECOUNT) % LASTPINTIMECOUNT];
      long timeTurnR = now - last_pinRTimes[(pinRTimeIdx - HITCOUNTTURN + LASTPINTIMECOUNT) % LASTPINTIMECOUNT];

      long timeLastL = now - last_pinLTimes[pinLTimeIdx];
      long timeLastR = now - last_pinRTimes[pinRTimeIdx];
      
      if (timeTurnL  < TIMETURN && timeLastR > TIMETURN)
      {
        direction = 270;
        speed = 128;
      }
      else if (timeTurnR  < TIMETURN && timeLastL > TIMETURN)
      {
        direction = 90;
        speed = 128;
      }
    }

    LimitPublishGo(direction, speed, SEND_INTERVAL + SEND_INTERVAL_ADD);
  }
};