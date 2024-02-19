//////////////////////////////////////////

#include <Arduino.h>
#include <EEPROM.h>
#include <string>

using namespace std;

#include "EepromConfig.h"

//////////////////////////////////////////

#define SIGNATURE 0x87651234

int EepromConfig::WriteStringToEEPROM(int addrOffset, const String &strToWrite)
{
  uint8_t len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}

int EepromConfig::ReadStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}

String *EepromConfig::ReadStrings()
{
  int idx = _eepromIdx;

  uint32_t signature;

  EEPROM.get<uint32_t>(_eepromIdx, signature);
  idx += sizeof(uint32_t);

  uint8_t canRead = 0;
  EEPROM.get<uint8_t>(idx, canRead);
  idx += sizeof(uint8_t);

  if (signature != SIGNATURE)
  {
    canRead = 0;
  }

  for (uint8_t i = 0; i < canRead; i++)
  {
    idx = ReadStringFromEEPROM(idx, &_stringBuffer[i]);
  }

  for (uint8_t i = canRead; i < _count; i++)
  {
      _stringBuffer[i] = "";
  }

  return _stringBuffer;
}

void EepromConfig::WriteStrings(String stringsToWrite[])
{
  int idx = _eepromIdx;

  EEPROM.put<uint32_t>(_eepromIdx, SIGNATURE);
  idx += sizeof(uint32_t);

  EEPROM.put<uint8_t>(idx, _count);
  idx += sizeof(uint8_t);

  for (uint8_t i = 0; i < _count; i++)
  {
    idx = WriteStringToEEPROM(idx, stringsToWrite[i]);
  }
  EEPROM.commit();
}

String EepromConfig::ReadString(int idx)
{
  if (idx >= _count)
  {
      return "";
  }

	return ReadStrings()[idx];
}
