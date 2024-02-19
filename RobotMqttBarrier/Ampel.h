/*--------------------------------------------------------------
         HTBLA-Leonding / Class: 1xHIF
  --------------------------------------------------------------
               Musterlösung
  --------------------------------------------------------------
  Description: Ampel (rot, gelb grün) OO Version
  --------------------------------------------------------------
*/

#pragma once

class Ampel
{
public:
  enum Phases {
    Red = 0,
    RedYellow,
    Green,
    GreenBlinkOff1,
    GreenBlinkOn1,
    GreenBlinkOff2,
    GreenBlinkOn2,
    GreenBlinkOff3,
    GreenBlinkOn3,
    GreenBlinkOff4,
    GreenBlinkOn4,
    Yellow
  };

private:
  uint8_t _redPin;
  uint8_t _yellowPin;
  uint8_t _greenPin;
  uint8_t _index;

  uint32_t _untilTime;
  uint32_t _phaseStartTime;
  uint32_t _stayOnRedOrGreenTime;
  Phases _currentPhase;

  struct Phase
  {
  public:
    bool redOn;
    bool yellowOn;
    bool greenOn;
    uint32_t duration;
  };

  static Phase _phases[];

public:
  Ampel(uint8_t redPin, uint8_t yellowPin, uint8_t greenPin, uint8_t index)
  {
    _redPin = redPin;
    _yellowPin = yellowPin;
    _greenPin = greenPin;
    _currentPhase = Phases::Red;
    _index = index;
  }

  void Init()
  {
    pinMode(_redPin, OUTPUT);
    pinMode(_yellowPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    SetPhase();
  }

  void Poll()
  {
    if (_untilTime <= millis())
    {
      SetNextPhase();
    }
  }

  void SetPhase(Phases phase)
  {
    _currentPhase = phase;
    SetPhase();
  }

  bool ToRed(uint32_t stayOnGreenTime = 0)
  {
    if (IsGotoRed())
    {
      return true;
    }
    else if (IsGotoGreen())
    {
      _stayOnRedOrGreenTime = stayOnGreenTime;
      return true;
    }
    else if (_currentPhase == Phases::Green)
    {
      _untilTime = millis() + max(stayOnGreenTime,(uint32_t) 1000);
      return true;
    }
    return false;
  }

  bool ToGreen(uint32_t stayOnRedTime = 0)
  {
    if (IsGotoGreen())
    {
      return true;
    }
    else if (IsGotoRed())
    {
      _stayOnRedOrGreenTime = stayOnRedTime;
      return true;
    }
    else if (_currentPhase == Phases::Red)
    {
      _untilTime = millis() + max(stayOnRedTime,(uint32_t)1000);
      return true;
    }
    return false;
  }

  bool IsRed() const
  {
    return _currentPhase == Phases::Red;
  }
  bool IsGreen() const
  {
    return _currentPhase >= Phases::Green &&
           _currentPhase <= Phases::GreenBlinkOn4;
  }

  bool IsGotoRed() const
  {
    return _currentPhase > Phases::Green;
  }

  bool IsGotoGreen() const
  {
    return _currentPhase > Phases::Red && _currentPhase < Phases::Green;
  }

  uint32_t PhaseSinceTime() const
  {
    return millis() - _phaseStartTime;
  }

private:
  void SetNextPhase()
  {
    _currentPhase = (enum Phases)(_currentPhase + 1);
    if (_currentPhase > Phases::Yellow)
    {
      _currentPhase = Phases::Red;
    }
    SetPhase();
  }

  void SetPhase();
};
