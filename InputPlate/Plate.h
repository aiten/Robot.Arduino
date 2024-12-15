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

  EState state = WaitForLeftOn;
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
    int lPin = digitalRead(LEFT_PIN);
    int rPin = digitalRead(RIGHT_PIN);

    if (lPinOld != lPin)
    {
      if (lPin==HIGH) // released
      {
        static long timePinLChanged = 0;
        long timeForLChanged = millis() - timePinLChanged;

        if (timeForLChanged > PRELLEN_TIME)
        {
          timePinLChanged = millis();
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
        long timeForRChanged = millis() - timePinRChanged;

        if (timeForRChanged > PRELLEN_TIME)
        {
          timePinRChanged = millis();
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

    static int pinForState[] = {LEFT_PIN, RIGHT_PIN, RIGHT_PIN, LEFT_PIN};
    static int waitForState[] = {LOW, HIGH, LOW, HIGH};

    if (digitalRead(pinForState[state]) == waitForState[state])
    {
      state = (EState)((state + 1) % LastState);
      long timeInState = millis() - stateSinceTime;
      stateSinceTime = millis();

      if (state == WaitForRightOff || state == WaitForLeftOff)
      {
        static long stepSinceTime = 0;
        long timeStep = millis() - stepSinceTime;
        stepSinceTime = millis();

        last_stepTimes[stepTimeIdx] = timeStep;
        stepTimeIdx = (stepTimeIdx + 1) % LASTTIMECOUNT;

        int totalTime=0;

        for (int i = 0; i < LASTTIMECOUNT; ++i) 
        {
          totalTime += last_stepTimes[i];
          if (last_stepTimes[i] == 0)
          {
            totalTime += MAXTIME;
          }
        }

        int avgTime = totalTime / LASTTIMECOUNT;
        lastSpeed = map(avgTime,MAXTIME,MINTIME,MIN_SPEED,MAX_SPEED);
        
        if (lastSpeed > MAX_SPEED) 
        {
          lastSpeed = MAX_SPEED;
        }
        if (lastSpeed < 0) 
        {
          lastSpeed = 0;
        }
      }
    }
    else
    {
      if (millis() - stateSinceTime > MAXTIME)
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

    if (millis() < until || !isOn)
    {
      return;
    }

    until = millis() + SEND_INTERVAL;

    // check for move left/right
    if (speed == 0)
    {
      long timeTurnL = millis() - last_pinLTimes[(pinLTimeIdx - HITCOUNTTURN + LASTPINTIMECOUNT) % LASTPINTIMECOUNT];
      long timeTurnR = millis() - last_pinRTimes[(pinRTimeIdx - HITCOUNTTURN + LASTPINTIMECOUNT) % LASTPINTIMECOUNT];

      long timeLastL = millis() - last_pinLTimes[pinLTimeIdx];
      long timeLastR = millis() - last_pinRTimes[pinRTimeIdx];
      
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