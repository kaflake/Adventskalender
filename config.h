#ifndef CONFIG_H
#define CONFIG_H

#define def_printTemperature // print temperature on time output
#define def_printTime // print time while on loop
#define def_testLed

#define CONTROL_BUTTON_PIN A0
#define CHRISTMAS_BUTTON_PIN 4
#define CHRISTMAS_LED 12
#define MP3_SERIAL_RX 2
#define MP3_SERIAL_TX 3
// LED_BUILTIN is pin 13 to test

#define DAY_TRACK_START 1 // 24 daytracks needed
#define OHTANNENBAUM_TRACK 25
#define CLOCK_TRACK 26 // kuckucksuhr z.B.
#define CHRISTMAS_TRACK 27
#define NOTALLOWED_TRACK 28 // shame on you z.b.
#define NEWYEAR_TRACKS_START 29

const unsigned long DOOR_REACTION_TIME(500); // ms die der schalte braucht bis er die tür als geschlossen oder offen definiert

NewYearTrack newYearTrackList[] = 
{
    // https://www.timeanddate.de/countdown/neujahr-zeitzonen
    {31, 11, 0, 29}, // Samoa und Weihnachtsinsel/Kiribati   	Kiritimati, Apia, Salelologa (Savai'i)
    {31, 11, 15, 30}, // Chatham-Inseln/Neuseeland   Chatham-Inseln
    {31, 12, 0, 31}, // Neuseeland mit Ausnahmen und 5 weitere	Auckland, Suva, Wellington, Nukuʻalofa
    {31, 13, 0, 32}, // Kleines Gebiet in Russland und 6 weitere	Anadyr, Funafuti, Yaren, Tarawa, Majuro
    {31, 14, 0, 33}, // Großteil von Australien und 8 weitere	Melbourne, Sydney, Canberra, Honiara
    {31, 14, 30, 34}, // Kleines Gebiet in Australien	Adelaide, Broken Hill, Ceduna
    {31, 15, 0, 35}, // Queensland/Australien und 6 weitere	Brisbane, Port Moresby, Hagåtña
    {31, 15, 30, 36}, // Northern Territory/Australien	Darwin, Alice Springs, Tennant Creek
    {31, 16, 0, 37}, // Japan, Südkorea und 5 weitere	Tokio, Seoul, Pjöngjang, Dili, Ngerulmud
    {31, 16, 15, 38}, // Western Australia/Australien	Eucla
    {31, 17, 0, 39}, // China, Philippinen und 10 weitere	Peking, Hongkong, Manila, Singapur
    {31, 18, 0, 40}, // Großteil von Indonesien und 8 weitere	Jakarta, Bangkok, Hanoi, Phnom Penh
    {31, 18, 30, 41}, // Myanmar und Kokosinseln	Rangun, Naypyidaw, Mandalay, Bantam
    {31, 19, 0, 42}, // Bangladesch und 6 weitere	Dhaka, Almaty, Bischkek, Thimphu, Astana
    {31, 19, 15, 43}, // Nepal	Kathmandu, Pokhara, Biratnagar, Dharan
    {31, 19, 30, 44}, // Indien und Sri Lanka	Neu-Delhi, Mumbai, Kalkutta, Bangalore
    {31, 20, 0, 45}, // Pakistan und 8 weitere	Taschkent, Islamabad, Lahore, Karatschi
    {31, 20, 30, 46}, // Afghanistan	Kabul, Kandahar, Mazār-i Scharif, Herat
    {31, 21, 0, 47}, // Aserbaidschan und 8 weitere	Dubai, Abu Dhabi, Muscat, Port Louis
    {31, 21, 30, 48}, // Iran	Teheran, Rasht, Isfahãn, Maschhad, Täbris
    {31, 22, 0, 49}, // Moskau/Russland und 22 weitere	Moskau, Ankara, Bagdad, Nairobi
    {31, 23, 0, 50}, // Griechenland und 31 weitere	Kairo, Athen, Bukarest, Johannesburg
    {1, 0, 0, 51}, // Deutschland und 46 weitere	Brüssel, Madrid, Paris, Rom, Algier
    {1, 1, 0, 52}, // Großbritannien und 23 weitere	London, Dublin, Lissabon, Accra
    {1, 2, 0, 53}, // Cabo Verde und 2 weitere	Praia, Ponta Delgada, Ittoqqortoormiit
    {1, 3, 0, 54}, // Brasilien (manche Regionen) und Südgeorgien und die Südlichen Sandwichinseln	Rio de Janeiro, São Paulo, Brasília
    {1, 4, 0, 55}, // Argentinien und 10 weitere	Buenos Aires, Santiago de Chile, Asunción
    {1, 4, 30, 56}, // Neufundland und Labrador/Kanada	St. John's, Mary's Harbour
    {1, 5, 0, 57}, // Einige Gebiete von Kanada und 28 weitere	Caracas, La Paz, San Juan, Santo Domingo
    {1, 6, 0, 58}, // USA (manche Regionen) und 14 weitere	New York, Washington DC, Detroit, Havanna
    {1, 7, 0, 59}, // USA (manche Regionen) und 9 weitere	Mexiko-Stadt, Chicago, Guatemala-Stadt
    {1, 8, 0, 60}, // Einige Gebiete von USA und 2 weitere	Calgary, Denver, Edmonton, Phoenix
    {1, 9, 0, 61}, // USA (manche Regionen) und 4 weitere	Los Angeles, San Francisco, Las Vegas
    {1, 10, 0, 62}, // Alaska/USA und Französisch-Polynesien (manche Regionen)	Anchorage, Fairbanks, Juneau, Unalaska
    {1, 10, 30, 63}, // Marquesas/Französisch-Polynesien	Taiohae
    {1, 11, 0, 64}, //	Kleines Gebiet in USA und 2 weitere	Honolulu, Rarotonga, Adak, Papeete
    {1, 12, 0, 65}, //	American Samoa und 2 weitere	Alofi, Midway, Pago Pago
    {1, 13, 0, 66} // Großteil von US Minor Outlying Islands	Bakerinsel, Howlandinsel
}

#endif