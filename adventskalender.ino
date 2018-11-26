// Set the date and time by entering the following on the Arduino
// serial monitor:
//  year,month,day,hour,minute,second,
//
// Where
//  year can be two or four digits,
//  month is 1-12,
//  day is 1-31,
//  hour is 0-23, and
//  minute and second are 0-59.
//
// Entering the final comma delimiter (after "second") will avoid a
// one-second timeout and will allow the RTC to be set more accurately.
//
// No validity checking is done, invalid values or incomplete syntax
// in the input will result in an incorrect RTC setting.

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/
#include <JC_Button.h>      // https://github.com/JChristensen/JC_Button
#include <DFMiniMp3.h>      // https://github.com/Makuna/DFMiniMp3
#include <SoftwareSerial.h>

#include "general.h"
#include "config.h"
//#include "Mp3Notify.h"
class Mp3Notify {
public:
  static void OnError(uint16_t errorCode) {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(uint16_t track) {
    Serial.print("Track beendet");
    Serial.println(track);
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

SoftwareSerial mp3SoftwareSerial(MP3_SERIAL_RX, MP3_SERIAL_TX);
static DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(mp3SoftwareSerial);
Button controlButton(CONTROL_BUTTON_PIN);
Button christmasButton(CHRISTMAS_BUTTON_PIN);

static time_t actualTime;
static time_t lastClockTime;

void setup()
{
    Serial.begin(115200);
    initRtc();
    initIos();
}

void initRtc() 
{
    // setSyncProvider() causes the Time library to synchronize with the
    // external RTC by calling RTC.get() every five minutes by default.
    setSyncProvider(RTC.get);
    Serial << F("RTC Sync");
    if (timeStatus() != timeSet) Serial << F(" FAIL!");
    Serial << endl;
}

void initMp3() 
{
    mp3.begin();
    mp3.setVolume(20);
}

void initIos()
{
    pinMode(CHRISTMAS_LED, OUTPUT);
    controlButton.begin();
    christmasButton.begin();

    #ifdef def_testLed
        pinMode(LED_BUILTIN, OUTPUT);
    #endif
}

void loop()
{
    actualTime = now;
    mp3.loop(); 

    // if serial input set it as time
    setRtcFromSerialLoop();

    #ifdef def_printTime
        printTimeToSerial();
    #endif

    readButtonsLoop();
    controlButtonLoop();

    #ifdef def_testLed
        if(christmasButton.wasReleased())
        {
            digitalWrite(LED_BUILTIN, true);
        }
        else
        {
            digitalWrite(LED_BUILTIN, false);    
        }
    #endif

    christmasDoorLoop();
    happyNewYearLoop();
    kuckucksUhrLoop();
}

void readButtonsLoop() 
{
    controlButton.read();
    christmasButton.read();
}

void controlButtonLoop()
{
    if(controlButton.wasReleased())
    {
        playTrackOfTheDay();
    }
}

void playTrackOfTheDay() 
{
    int actualDay = day();
    if(month() == 12 && actualDay <= 24)
    {
        int dayTrackIndex = actualDay - 1;
        playMp3Track(dayTrackIndex + DAY_TRACK_START);
    } else 
    {
        playMp3Track(OHTANNENBAUM_TRACK);
    }
}

void christmasDoorLoop()
{
    if(christmasButton.pressedFor(DOOR_REACTION_TIME)) // reed is open for LONG_PRESS time
    {
        onChristmasDoorOpend(); 
    }

    if(christmasButton.pressedFor(DOOR_REACTION_TIME))
    {
        onChristmasDoorClosed();
    }
}

void onChristmasDoorOpend()
{
    playChristmasTrack();
    activateChristmasLed();     
}

void onChristmasDoorClosed()
{
    deactivateChristmasLed();
}

void playChristmasTrack()
{
    if(month() == 12 && day() >= 24 || month() == 1 && day() <= 6) // es ist weihnachten oder danach soll es noch gehen bis 3 König
    {        
        playMp3Track(CHRISTMAS_TRACK);
    } else {
        playMp3Track(NOTALLOWED_TRACK);
        Serial << 'Spiele: heut ist aber nicht weihnachten mach das schnell wieder zu' << endl;
    }
}

void activateChristmasLed()
{
    digitalWrite(CHRISTMAS_LED, HIGH);
}

void deactivateChristmasLed()
{
    digitalWrite(CHRISTMAS_LED, LOW);
}

void happyNewYearLoop() 
{
    int newYearTrack = getNewYearTrack(); // ggf get newyear oder kuckuck track. ziel: net nochmal now auslösen, das immer nur einmal. ggf. now auch speichern.
    if(newYearTrack > 0) {
        setLastClockTime();
        playAdvertisementTrack(newYearTrack);
    }
}

// -1 if not, else return the track
int getNewYearTrack() 
{
    if((day(actualTime) == 31 && month(actualTime) == 12 || day(actualTime) == 1 && month(actualTime) == 1) &&
        (minute(actualTime) == 0 || minute(actualTime) == 15 || minute(actualTime) == 30)) // only at this minutes its possible new year
    {
        int arraySize = sizeof(newYearTrackList) / sizeof(newYearTrackList[0]);
        for (int i = 0; i < arraySize; i++)
        {
            // return newYearTrackList[i].Track;
            return NEWYEAR_TRACKS_START + i;
        }
    }

    return -1;
}

void kuckucksUhrLoop() 
{    
    // da happy new year davor schon lastClockTime setzt ersetzt dies dann die kuckucksuhr
    if(minute(actualTime) == 0 && hour(actualTime) != hour(lastClockTime))
    {
        setLastClockTime();
        playAdvertisementTrack(CLOCK_TRACK);
    }
}

void setLastClockTime()
{
    lastClockTime = actualTime;
}

void playMp3Track(int track) 
{
    // TODO auch testen ob schon gespielt wird, dann ignoriere das
    Serial << 'Play MP3Track ' << track << endl;
    mp3.playMp3FolderTrack(track);
}

void playAdvertisementTrack(int track) 
{
    // TODO auch testen ob schon gespielt wird, dann ignoriere das
    Serial << 'Play AdvertismentTrack ' << track << endl;
    mp3.playAdvertisement(track);
}

void setRtcFromSerialLoop()
{
    tmElements_t tm;
    
    // check for input to set the RTC, minimum length is 12, i.e. yy,m,d,h,m,s
    if (Serial.available() >= 12) {
        // note that the tmElements_t Year member is an offset from 1970,
        // but the RTC wants the last two digits of the calendar year.
        // use the convenience macros from the Time Library to do the conversions.
        int y = Serial.parseInt();
        if (y >= 100 && y < 1000)
            Serial << F("Error: Year must be two digits or four digits!") << endl;
        else {
            if (y >= 1000)
                tm.Year = CalendarYrToTm(y);
            else    // (y < 100)
                tm.Year = y2kYearToTm(y);
            tm.Month = Serial.parseInt();
            tm.Day = Serial.parseInt();
            tm.Hour = Serial.parseInt();
            tm.Minute = Serial.parseInt();
            tm.Second = Serial.parseInt();
            time_t t = makeTime(tm);
            RTC.set(t);        // use the time_t value to ensure correct weekday is set
            setTime(t);
            Serial << F("RTC set to: ");
            printDateTime(t);
            Serial << endl;
            // dump any extraneous input
            while (Serial.available() > 0) Serial.read();
         }
    }
}

void printTimeToSerial()
{
    // only print time once, only print again if changed (new second)
    static time_t tLast;
    time_t t;
    t = now();
    if (t != tLast) {
        tLast = t;
        printDateTime(t);
        #ifdef def_printTemperature
            if (second(t) == 0) {
                float c = RTC.temperature() / 4.;
                Serial << F("  ") << c << F(" C  ");
            }
        #endif
        Serial << endl;
    }
}

// print date and time to Serial
void printDateTime(time_t t)
{
    printDate(t);
    Serial << ' ';
    printTime(t);
}

// print time to Serial
void printTime(time_t t)
{
    printI00(hour(t), ':');
    printI00(minute(t), ':');
    printI00(second(t), ' ');
}

// print date to Serial
void printDate(time_t t)
{
    printI00(day(t), 0);
    Serial << monthShortStr(month(t)) << _DEC(year(t));
}

// Print an integer in "00" format (with leading zero),
// followed by a delimiter character to Serial.
// Input value assumed to be between 0 and 99.
void printI00(int val, char delim)
{
    if (val < 10) Serial << '0';
    Serial << _DEC(val);
    if (delim > 0) Serial << delim;
    return;
}
