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
    actualTime = now;
    mp3.loop(); 

    // if serial input set it as time
    // für setrtc reicht der speicherplatz nicht mehr. zum setzen der zeit ein extra sketch benutzen (in der DS3232RTC lib in den beispielen vorhanden)
    // setRtcFromSerialLoop();

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

    deactivateClockAndNewYearsLedAfterAMinute();
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
        //Serial << 'Spiele: heut ist aber nicht weihnachten mach das schnell wieder zu' << endl;
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
        Serial << 'Is not playing -> No Advertisment needed' << endl;
        playMp3Track(track);
    }
}

void playMp3Track(int track) 
{
    // TODO auch testen ob schon gespielt wird, dann ignoriere das
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
    // only print time once, only print again if changed (new second)
    static time_t tLast;
    time_t t;
    t = now();
    if (t != tLast) {
        tLast = t;
        printDateTime(t);
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
