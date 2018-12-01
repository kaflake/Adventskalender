#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"
#include "general.h"

#define def_printTimeLoop // print time while on loop
//#define def_testLed
//#define def_setTime

#define MP3_SERIAL_RX 2
#define MP3_SERIAL_TX 3
#define MP3_BUSY_PIN 4
#define CONTROL_BUTTON_PIN 5
#define CHRISTMAS_BUTTON_PIN 6
#define LED_STRIPE_PIN 7
// Uhr an I2C -> SCL auf A5 - SDA auf A4
// LED_BUILTIN is pin 13 to test

#define VOLUME 25

#define LED_STRIPE_COUNT 30
#define LED_STRIPE_CHRISTMAS_SEGMENT 0
#define LED_STRIPE_CHRISTMAS_SEGMENT_START 0
#define LED_STRIPE_CHRISTMAS_SEGMENT_COUNT 6 // das im türchen 24
#define LED_STRIPE_TREE_SEGMENT 1
#define LED_STRIPE_TREE_SEGMENT_START 6
#define LED_STRIPE_TREE_SEGMENT_COUNT 24 // das an der spitze

#define DAY_TRACK_START 1 // 24 daytracks needed
#define OHTANNENBAUM_TRACK 25
#define CLOCK_TRACK 26 // kuckucksuhr z.B.
#define CHRISTMAS_TRACK 27
#define NOTALLOWED_TRACK 28 // shame on you z.b.
#define NEWYEAR_TRACKS_START 29 // -> bis 66

const unsigned long DOOR_REACTION_TIME(500); // ms die der schalte braucht bis er die tür als geschlossen oder offen definiert

const NewYearTrack newYearTrackList[] = 
{
    // https://www.timeanddate.de/countdown/neujahr-zeitzonen
    {31, 11, 0}, // Samoa und Weihnachtsinsel/Kiribati   	Kiritimati, Apia, Salelologa (Savai'i)
    {31, 11, 15}, // Chatham-Inseln/Neuseeland   Chatham-Inseln
    {31, 12, 0}, // Neuseeland mit Ausnahmen und 5 weitere	Auckland, Suva, Wellington, Nukuʻalofa
    {31, 13, 0}, // Kleines Gebiet in Russland und 6 weitere	Anadyr, Funafuti, Yaren, Tarawa, Majuro
    {31, 14, 0}, // Großteil von Australien und 8 weitere	Melbourne, Sydney, Canberra, Honiara
    {31, 14, 30}, // Kleines Gebiet in Australien	Adelaide, Broken Hill, Ceduna
    {31, 15, 0}, // Queensland/Australien und 6 weitere	Brisbane, Port Moresby, Hagåtña
    {31, 15, 30}, // Northern Territory/Australien	Darwin, Alice Springs, Tennant Creek
    {31, 16, 0}, // Japan, Südkorea und 5 weitere	Tokio, Seoul, Pjöngjang, Dili, Ngerulmud
    {31, 16, 15}, // Western Australia/Australien	Eucla
    {31, 17, 0}, // China, Philippinen und 10 weitere	Peking, Hongkong, Manila, Singapur
    {31, 18, 0}, // Großteil von Indonesien und 8 weitere	Jakarta, Bangkok, Hanoi, Phnom Penh
    {31, 18, 30}, // Myanmar und Kokosinseln	Rangun, Naypyidaw, Mandalay, Bantam
    {31, 19, 0}, // Bangladesch und 6 weitere	Dhaka, Almaty, Bischkek, Thimphu, Astana
    {31, 19, 15}, // Nepal	Kathmandu, Pokhara, Biratnagar, Dharan
    {31, 19, 30}, // Indien und Sri Lanka	Neu-Delhi, Mumbai, Kalkutta, Bangalore
    {31, 20, 0}, // Pakistan und 8 weitere	Taschkent, Islamabad, Lahore, Karatschi
    {31, 20, 30}, // Afghanistan	Kabul, Kandahar, Mazār-i Scharif, Herat
    {31, 21, 0}, // Aserbaidschan und 8 weitere	Dubai, Abu Dhabi, Muscat, Port Louis
    {31, 21, 30}, // Iran	Teheran, Rasht, Isfahãn, Maschhad, Täbris
    {31, 22, 0}, // Moskau/Russland und 22 weitere	Moskau, Ankara, Bagdad, Nairobi
    {31, 23, 0}, // Griechenland und 31 weitere	Kairo, Athen, Bukarest, Johannesburg
    {1, 0, 0}, // Deutschland und 46 weitere	Brüssel, Madrid, Paris, Rom, Algier
    {1, 1, 0}, // Großbritannien und 23 weitere	London, Dublin, Lissabon, Accra
    {1, 2, 0}, // Cabo Verde und 2 weitere	Praia, Ponta Delgada, Ittoqqortoormiit
    {1, 3, 0}, // Brasilien (manche Regionen) und Südgeorgien und die Südlichen Sandwichinseln	Rio de Janeiro, São Paulo, Brasília
    {1, 4, 0}, // Argentinien und 10 weitere	Buenos Aires, Santiago de Chile, Asunción
    {1, 4, 30}, // Neufundland und Labrador/Kanada	St. John's, Mary's Harbour
    {1, 5, 0}, // Einige Gebiete von Kanada und 28 weitere	Caracas, La Paz, San Juan, Santo Domingo
    {1, 6, 0}, // USA (manche Regionen) und 14 weitere	New York, Washington DC, Detroit, Havanna
    {1, 7, 0}, // USA (manche Regionen) und 9 weitere	Mexiko-Stadt, Chicago, Guatemala-Stadt
    {1, 8, 0}, // Einige Gebiete von USA und 2 weitere	Calgary, Denver, Edmonton, Phoenix
    {1, 9, 0}, // USA (manche Regionen) und 4 weitere	Los Angeles, San Francisco, Las Vegas
    {1, 10, 0}, // Alaska/USA und Französisch-Polynesien (manche Regionen)	Anchorage, Fairbanks, Juneau, Unalaska
    {1, 10, 30}, // Marquesas/Französisch-Polynesien	Taiohae
    {1, 11, 0}, //	Kleines Gebiet in USA und 2 weitere	Honolulu, Rarotonga, Adak, Papeete
    {1, 12, 0}, //	American Samoa und 2 weitere	Alofi, Midway, Pago Pago
    {1, 13, 0} // Großteil von US Minor Outlying Islands	Bakerinsel, Howlandinsel
};

#endif
