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
Button christmasButton(CHRISTMAS_BUTTON_PIN, 25, true, false);

static time_t actualTime;
static time_t lastClockTime;
static unsigned long treeLedStartTime = 0;
static bool treeLedActive = true;
static bool doorLastClosed = false; // startet quasi als offen dadurch reagiert es aufs erste öffnen nicht
static int lastNewYearTrack = 0; // 0 no was played -> default on button push

void setup()
{
    Serial.begin(115200);
    initRtc();
    initIos();
    initMp3();
    initLedStrip();
    // printTimeToSerial();
}

void initRtc() 
{
    // setSyncProvider() causes the Time library to synchronize with the
    // external RTC by calling RTC.get() every five minutes by default.
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) 
    {
      //Serial << "RTC Sync FAIL!" << endl;
      Serial.println("RTC Sync FAIL");
    }
}

void initMp3() 
{
    mp3.begin();
    mp3.setVolume(VOLUME);
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

    
    deactivateChristmasLed();
    ws2812fx.start();
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
  actualTime = now();
  readButtonsLoop();
  mp3.loop(); 
  ws2812fx.service();
  controlButtonLoop();
  christmasDoorLoop();
  happyNewYearLoop();
  kuckucksUhrLoop();

  deactivateTreeLedAfterRunOut();
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
      if(lastNewYearTrack > 0 && isNewYearDateArea()) {
          playNewYear(lastNewYearTrack);
      } 
      else
      {
        playTrackOfTheDay();
      }
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

    activateChristmasTreeLed();
}

void christmasDoorLoop()
{
    if(christmasButton.pressedFor(DOOR_REACTION_TIME) && doorLastClosed) // reed is open for LONG_PRESS time
    {
        onChristmasDoorOpend();
        doorLastClosed = false;
    } else if(christmasButton.releasedFor(DOOR_REACTION_TIME) && !doorLastClosed)
    {
        onChristmasDoorClosed();
        doorLastClosed = true;
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
    ws2812fx.setSegment(LED_STRIPE_CHRISTMAS_SEGMENT,  
    LED_STRIPE_CHRISTMAS_SEGMENT_START, 
    LED_STRIPE_CHRISTMAS_SEGMENT_START + LED_STRIPE_CHRISTMAS_SEGMENT_COUNT - 1, 
    FX_MODE_FIRE_FLICKER,
    ORANGE,
    1000,
    false);
}

void deactivateChristmasLed()
{
    ws2812fx.setSegment(LED_STRIPE_CHRISTMAS_SEGMENT,  
    LED_STRIPE_CHRISTMAS_SEGMENT_START, 
    LED_STRIPE_CHRISTMAS_SEGMENT_START + LED_STRIPE_CHRISTMAS_SEGMENT_COUNT - 1, 
    FX_MODE_STATIC,
    BLACK,
    1000,
    false);
}

void happyNewYearLoop() 
{
    if(minute(actualTime) != minute(lastClockTime) || hour(actualTime) != hour(lastClockTime))
    {
      int newYearTrack = getNewYearTrack(); // ggf get newyear oder kuckuck track. ziel: net nochmal now auslösen, das immer nur einmal. ggf. now auch speichern.
      lastNewYearTrack = newYearTrack;
      if(newYearTrack > 0) {
          setLastClockTime();
          playNewYear(newYearTrack);
      }
    }
}

void playNewYear(int newYearTrack)
{
  activateNewYearTreeLed();
  playAdvertisementTrack(newYearTrack);
}

// -1 if not, else return the track
int getNewYearTrack() 
{
    if(isNewYearDateArea() &&
        (minute(actualTime) == 0 || minute(actualTime) == 15 || minute(actualTime) == 30)) // only at this minutes its possible new year
    {
        int arraySize = sizeof(newYearTrackList) / sizeof(newYearTrackList[0]);
        for (int i = 0; i < arraySize; i++)
        {
            if(newYearTrackList[i].Day == day(actualTime) && newYearTrackList[i].Hour == hour(actualTime) && newYearTrackList[i].Minute == minute(actualTime))
            {
              return NEWYEAR_TRACKS_START + i;
            }
        }
    }

    return -1;
}

bool isNewYearDateArea()
{
  return ((day(actualTime) == 31 && month(actualTime) == 12) || (day(actualTime) == 1 && month(actualTime) == 1));
}

void activateNewYearTreeLed() 
{
    setTreeLedActive();
    // parameters: index, start, stop, mode, color, speed, reverse
    ws2812fx.setSegment(LED_STRIPE_TREE_SEGMENT,  
      LED_STRIPE_TREE_SEGMENT_START, 
      LED_STRIPE_TREE_SEGMENT_START + LED_STRIPE_TREE_SEGMENT_COUNT - 1, 
      FX_MODE_FIREWORKS_RANDOM,
      RED,
      1000,
      false);
}

void kuckucksUhrLoop() 
{    
    // da happy new year davor schon lastClockTime setzt ersetzt dies dann die kuckucksuhr
    if(minute(actualTime) == 0 && hour(actualTime) != hour(lastClockTime))
    {
        setLastClockTime();
        playAdvertisementTrack(CLOCK_TRACK);
        activateChristmasTreeLed();        
    }
}

void setLastClockTime()
{
    lastClockTime = actualTime;
}

void activateChristmasTreeLed() 
{
    setTreeLedActive();
    // ws2812fx.setBrightness(255);
    // parameters: index, start, stop, mode, color, speed, reverse
    ws2812fx.setSegment(LED_STRIPE_TREE_SEGMENT,  
      LED_STRIPE_TREE_SEGMENT_START, 
      LED_STRIPE_TREE_SEGMENT_START + LED_STRIPE_TREE_SEGMENT_COUNT - 1, 
      FX_MODE_MERRY_CHRISTMAS,
      RED,
      1000,
      false);
}

void setTreeLedActive()
{
    treeLedActive = true;
    treeLedStartTime = millis();
}

void deactivateTreeLedAfterRunOut()
{ 
    if(treeLedActive && (treeLedStartTime + MIN_TREE_LED_TIME) < (millis()) && !isPlaying())
    {
        ws2812fx.setSegment(LED_STRIPE_TREE_SEGMENT,  
          LED_STRIPE_TREE_SEGMENT_START, 
          LED_STRIPE_TREE_SEGMENT_START + LED_STRIPE_TREE_SEGMENT_COUNT - 1, 
          FX_MODE_STATIC,
          BLACK,
          1000,
          false);
        treeLedActive = false;
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
    return !digitalRead(MP3_BUSY_PIN);
}

void printTimeToSerial()
{
  static time_t tLast;
  time_t t = now();
  if (t != tLast) 
  {
    tLast = t;
  
    // digital clock display of the time
    //Serial.print(hour());
    printNumber(hour());
    printColon();
    printNumber(minute());
    printColon();
    printNumber(second());
    Serial << endl;
  }
}

// spart speicher in extra funktion
void printColon()
{
  Serial << ":";
}

// spart speicher in extra funktion
void printNumber(int number)
{
    Serial << number;    
}
