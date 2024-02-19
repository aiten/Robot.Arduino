#pragma once

//////////////////////////////////////////

class EepromConfig
{

private:

  const uint8_t _count;
  const int _eepromIdx;
  String* _stringBuffer;

public:
  EepromConfig(uint8_t count, int eepromIdx, String* stringBuffer) : _count(count), _eepromIdx(eepromIdx), _stringBuffer(stringBuffer)
  {
  }

  int WriteStringToEEPROM(int addrOffset, const String &strToWrite);
  int ReadStringFromEEPROM(int addrOffset, String *strToRead);

  String* ReadStrings();
  void WriteStrings(String stringsToWrite[]);

  String ReadString(int idx);
};
