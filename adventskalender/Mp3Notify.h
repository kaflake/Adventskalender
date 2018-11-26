#ifndef MP3NOTIFY_H
#define MP3NOTIFY_H

#include "Arduino.h"

class Mp3Notify {
public:
  static void OnError(uint16_t errorCode) {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print(F("Com Error "));
    Serial.println(errorCode);
  }
  static void OnPlayFinished(uint16_t track) {
    Serial.print(F("Track beendet"));
  }
  static void OnCardOnline(uint16_t code) {
    Serial.println(F("SD Karte online "));
  }
  static void OnCardInserted(uint16_t code) {
    Serial.println(F("SD Karte bereit "));
  }
  static void OnCardRemoved(uint16_t code) {
    Serial.println(F("SD Karte entfernt "));
  }
};

#endif
