#ifndef MP3NOTIFY_H
#define MP3NOTIFY_H

#include "Arduino.h"

class Mp3Notify {
public:
  static void OnError(uint16_t errorCode) {
    // see DfMp3_Error for code meaning    
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(uint16_t track) {
    Serial.print("Track beendet");
    Serial.println(track);
  }
  static void OnCardOnline(uint16_t code) {
    Serial.println("SD Karte online ");
  }
  static void OnCardInserted(uint16_t code) {
    Serial.println("SD Karte bereit ");
  }
  static void OnCardRemoved(uint16_t code) {
    Serial.println("SD Karte entfernt ");
  }
};

#endif
