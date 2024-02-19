/*--------------------------------------------------------------
         HTBLA-Leonding / Class: 1xHIF
  --------------------------------------------------------------
               Musterlösung
  --------------------------------------------------------------
  Description: Ampel (rot, gelb grün) OO Version
  --------------------------------------------------------------
*/

#include <ArduinoJson.h>

#include "config.h"

#include "Ampel.h"

Ampel::Phase Ampel::_phases[] = {
    {true, false, false, 0},    // Red = 0,
    {true, true, false, 1000},  // RedYellow,
    {false, false, true, 0},    // Green,
    {false, false, false, 500}, // GreenBlinkOff1,
    {false, false, true, 500},  // GreenBlinkOn1,
    {false, false, false, 500}, // GreenBlinkOff2,
    {false, false, true, 500},  // GreenBlinkOn2,
    {false, false, false, 500}, // GreenBlinkOff3,
    {false, false, true, 500},  // GreenBlinkOn3,
    {false, false, false, 500}, // GreenBlinkOff4,
    {false, false, true, 500},  // GreenBlinkOn4,
    {false, true, false, 2000}, // Yellow
};

void Ampel::SetPhase()
{
  digitalWrite(_redPin, _phases[_currentPhase].redOn);
  digitalWrite(_yellowPin, _phases[_currentPhase].yellowOn);
  digitalWrite(_greenPin, _phases[_currentPhase].greenOn);
  _phaseStartTime = millis();
  if (_phases[_currentPhase].duration == 0) // red or green
  {
    if (_stayOnRedOrGreenTime == 0xffffffff)
    {
      _untilTime = 0xffffffff; // ever
    }
    else
    {
      _untilTime = _phaseStartTime + _stayOnRedOrGreenTime;
      _stayOnRedOrGreenTime = 0xffffffff;
    }
  }
  else
  {
    _untilTime = _phaseStartTime + _phases[_currentPhase].duration;
  }

  JsonDocument doc;

  doc["phase"] = _currentPhase;
  doc["isRed"] = IsRed();
  doc["isGreen"] = IsGreen();

  String output;
  serializeJson(doc, output);

  client.publish(MQTT_STAT + "/phase" + String(_index), output.c_str());
}
