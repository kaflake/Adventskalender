#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/
#include <JC_Button.h>      // https://github.com/JChristensen/JC_Button
#include <DFMiniMp3.h>      // https://github.com/Makuna/DFMiniMp3
#include <SoftwareSerial.h>
#include <WS2812FX.h>

#include "general.h"
#include "config.h"
#include "Mp3Notify.h"

SoftwareSerial mp3SoftwareSerial(MP3_SERIAL_RX, MP3_SERIAL_TX);
static DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(mp3SoftwareSerial);
WS2812FX ws2812fx = WS2812FX(LED_STRIPE_COUNT, LED_STRIPE_PIN, NEO_GRB + NEO_KHZ800);
Button controlButton(CONTROL_BUTTON_PIN);
Button christmasButton(CHRISTMAS_BUTTON_PIN);

static time_t actualTime;
static time_t lastClockTime;
static bool clockLedActive = true;

void setup()
{
    Serial.begin(115200);
    initRtc();
    initIos();
    initMp3();
    initLedStrip();
    printTimeToSerial();
}

void initRtc() 
{
    // setSyncProvider() causes the Time library to synchronize with the
    // external RTC by calling RTC.get() every five minutes by default.
    setSyncProvider(RTC.get);
    Serial << "RTC Sync";
    if (timeStatus() != timeSet) Serial << " FAIL!";
    Serial << endl;
}

void initMp3() 
{
    mp3.begin();
    mp3.setVolume(20);
}

void initIos()
{
    pinMode(LED_STRIPE_PIN, OUTPUT);
    controlButton.begin();
    christmasButton.begin();

    pinMode(MP3_BUSY_PIN, INPUT);

    #ifdef def_testLed
        pinMode(LED_BUILTIN, OUTPUT);
    #endif
}

void initLedStrip()
{
    ws2812fx.init();
    ws2812fx.setBrightness(255);
}

void loop()
{      
    #ifdef def_setTime
    // if serial input set it as time
    // für setrtc reicht der speicherplatz nicht mehr. zum setzen der zeit ein extra sketch benutzen (in der DS3232RTC lib in den beispielen vorhanden)
    setRtcFromSerialLoop();    
    #endif

    #ifdef def_printTimeLoop
    printTimeToSerial();
    #endif

    #ifndef def_setTime
    defaultLoop();

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
    #endif
}

void defaultLoop()
{
  actualTime = now;
  readButtonsLoop();
  mp3.loop(); 
  controlButtonLoop();
  christmasDoorLoop();
  happyNewYearLoop();
  kuckucksUhrLoop();

  deactivateClockAndNewYearsLedAfterAMinute();
}

void setRtcFromSerialLoop()
{
  // als extra funktion braucht es mehr speicherplatz und ging net
  // input: yy,m,d,h,m,s, letztes komma sorgt dafür dass es gleich übernommen wird
  if (Serial.available() >= 12) 
  {
    time_t t;
    tmElements_t tm;
    int y = Serial.parseInt();
    tm.Year = y2kYearToTm(y);
    tm.Month = Serial.parseInt();
    tm.Day = Serial.parseInt();
    tm.Hour = Serial.parseInt();
    tm.Minute = Serial.parseInt();
    tm.Second = Serial.parseInt();
    t = makeTime(tm);
    RTC.set(t);        // use the time_t value to ensure correct weekday is set
    setTime(t);
    printTimeToSerial();

    while (Serial.available() > 0) Serial.read();
  }
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
    } 
    else 
    {
        playMp3Track(OHTANNENBAUM_TRACK);
    }
}

void christmasDoorLoop()
{
    if(christmasButton.pressedFor(DOOR_REACTION_TIME)) // reed is open for LONG_PRESS time
    {
        onChristmasDoorOpend(); 
    } else if(christmasButton.releasedFor(DOOR_REACTION_TIME))
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
    if((month() == 12 && day() >= 24) || (month() == 1 && day() <= 6)) // es ist weihnachten oder danach soll es noch gehen bis 3 König
    {        
        playMp3Track(CHRISTMAS_TRACK);
    } else {
        playMp3Track(NOTALLOWED_TRACK);
    }
}

void activateChristmasLed()
{
    // ws2812fx.setBrightness(255);
    // parameters: index, start, stop, mode, color, speed, reverse
    ws2812fx.setSegment(LED_STRIPE_TREE_SEGMENT,  
    LED_STRIPE_CHRISTMAS_SEGMENT_START, 
    LED_STRIPE_CHRISTMAS_SEGMENT_START + LED_STRIPE_CHRISTMAS_SEGMENT_COUNT - 1, 
    FX_MODE_FIRE_FLICKER,
    RED,
    1000,
    false);

    ws2812fx.start();
}

void deactivateChristmasLed()
{
    // alway deactivate all
    ws2812fx.stop();
}

void happyNewYearLoop() 
{
    int newYearTrack = getNewYearTrack(); // ggf get newyear oder kuckuck track. ziel: net nochmal now auslösen, das immer nur einmal. ggf. now auch speichern.
    if(newYearTrack > 0) {
        setLastClockTime();
        activateNewYearsLed();
        playAdvertisementTrack(newYearTrack);
    }
}

// -1 if not, else return the track
int getNewYearTrack() 
{
    if(((day(actualTime) == 31 && month(actualTime) == 12) || (day(actualTime) == 1 && month(actualTime) == 1)) &&
        (minute(actualTime) == 0 || minute(actualTime) == 15 || minute(actualTime) == 30)) // only at this minutes its possible new year
    {
        int arraySize = sizeof(newYearTrackList) / sizeof(newYearTrackList[0]);
        for (int i = 0; i < arraySize; i++)
        {
            return NEWYEAR_TRACKS_START + i;
        }
    }

    return -1;
}


void activateNewYearsLed() 
{
    clockLedActive = true;
    // ws2812fx.setBrightness(255);
    // parameters: index, start, stop, mode, color, speed, reverse
    ws2812fx.setSegment(LED_STRIPE_TREE_SEGMENT,  
    LED_STRIPE_TREE_SEGMENT_START, 
    LED_STRIPE_TREE_SEGMENT_START + LED_STRIPE_TREE_SEGMENT_COUNT - 1, 
    FX_MODE_FIREWORKS_RANDOM,
    RED,
    1000,
    false);

    ws2812fx.start();
}

void kuckucksUhrLoop() 
{    
    // da happy new year davor schon lastClockTime setzt ersetzt dies dann die kuckucksuhr
    if(minute(actualTime) == 0 && hour(actualTime) != hour(lastClockTime))
    {
        setLastClockTime();
        playAdvertisementTrack(CLOCK_TRACK);
        activateKuckucksuhrLed();        
    }
}

void setLastClockTime()
{
    lastClockTime = actualTime;
}

void activateKuckucksuhrLed() 
{
    clockLedActive = true;
    // ws2812fx.setBrightness(255);
    // parameters: index, start, stop, mode, color, speed, reverse
    ws2812fx.setSegment(LED_STRIPE_TREE_SEGMENT,  
    LED_STRIPE_TREE_SEGMENT_START, 
    LED_STRIPE_TREE_SEGMENT_START + LED_STRIPE_TREE_SEGMENT_COUNT - 1, 
    FX_MODE_MERRY_CHRISTMAS,
    RED,
    1000,
    false);

    ws2812fx.start();
}

void deactivateClockAndNewYearsLedAfterAMinute()
{
    if(clockLedActive)
    {
        if(minute(actualTime) != minute(lastClockTime))
        {
            ws2812fx.stop();
            clockLedActive = false;
        }
    }
}

void playAdvertisementTrack(int track) 
{
    if(isPlaying())
    {
        //Serial << 'Play AdvertismentTrack ' << track << endl;
        mp3.playAdvertisement(track);
    }
    else
    {
        //Serial << 'Is not playing -> No Advertisment needed' << endl;
        playMp3Track(track);
    }
}

void playMp3Track(int track) 
{
    //Serial << 'Play MP3Track ' << track << endl;
    mp3.playMp3FolderTrack(track);
}

bool isPlaying() 
{
    // kann etwas dauern bis busy erscheint, deshalb davor ein delay
    delay(500);
    return !digitalRead(MP3_BUSY_PIN);
}

void printTimeToSerial()
{
    // digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(' ');
    Serial.print(day());
    Serial.print('.');
    Serial.print(month());
    Serial.print('.');
    Serial.print(year());
    Serial.println();
}

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}
