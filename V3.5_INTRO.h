    DESIGN V3.5 
        Szükséges változtatások:
            //* az egész sisakot előrébb kell dönteni a pánthoz képest
                //? 3°-al döntöttem előrébb
            //* betét bool body-jának párhuzamosítás a tengelyekkel
            //* légszívótér és elektronika tér külön legyen
            //* rögzítő csavarok
            //* vezeték alagutak
            //* a mágneses ütköző a HEAD és visor között legyen kicsit befele fordítva, és ez adja a kontaktot a mikrofonnak
            //* hangszórót az akksi felé eltolni
            //* a visor-on lévő reed mégnest a belső fal felől lehessen behelyezni, így csavarral is lehet rögzíteni ragasztás helyett
            //* a touch bemenethez tartozó anya bool body-ja túl kicsi
            //* a COVER és HEAD részeket összekötő csavar bool body-ja túl kicsi
            //* a COVER-t tartó csavarok fejeinek bool body-ja túl kicsi
            //* ne legyen RGB LED
            //* szigetelés
            //* az IR sensor bool body-ja túl kicsi és fordítva van
            //* a visor két legszélső patentjét tartó csavar beleütközik felnyitáskor a HEAD-be
            //* a ventillátorokat alulról lehessen beszerelni
            //* a lámpához reed relé kell hogy csak akkor legyen benne feszültség ha rajta van a lámpa
            //* a lámpa túl könnyen lejön ezért növelni kell a felfekvő mélységet
            //* az akkumulátoron legyen a bekapcsoló gomb és egy battery charge kijelző
            //* az akkumulátort három részből kell megcsinálni a szerelhetőség miatt
            //* az akkumulátor bemenete USB-C legyen
            //* VISOR és HEAD közötti kétoldali kontaktmágnest el kell tolni az Y tengelyen egy kicsit
            //* a HEAD-hez illeszteni a VISOR-t
            //* a HEAD-hez illeszteni a COVER-t
            //* VISOR tengelyének bool body-ját újracsinálni, és a súrlódó alátéteket lecserélni nagyobbra
            //* HEAD és COVER legelső összetartó csavarjának a helye hiányzik
            //* VISOR felső végpontjában legyen a COVER-en egy bepattanós rész
            //* VISOR-ban lévő vezeték alagút nincs kidolgozva
            //* az akkumulátorhoz tartozó terminal block-ok legyenek rejtve
            //* pontosan meghatározni a power PCB méreteit1
            //* a felső perk mechanikai és villamos csatlakozása USB-vel (Z tengelyen zár) és két tengelyen (X és Y) alakzáró oldható kötéssel
            //* könnyítések


BUCK:
    LM2596 - big
BOOST:
    HW-668 - small
    CN6009 - big
    MT3608 - medium

         
    PCB V8
        Szükséges tesztek:
            //* az új mosfet-eket ki kell próbálni
            //* a lámpa túl nagy feszültségesét okoz, ezért változik a fényerő ha az egyiket levesszük. Ki kell próbálni másik buck convertert, vagy a jelenlegit máshogy használni. Esetleg a PWM transisor miatt lehet.
                //? a probléma az N-channel MOSFET-el volt NPN transistor-al már tökéletesen működött
                //? működő NPN transistor: NTE2343
                //? működik a motorral is
        Szükséges változtatások:
            //* új audio erősítő kell, amit a power pcb-re kell forrasztani
                //? R=22 k C=33 mikroF
            //* a külső feltöltéshez szükséges RTS pin a serial to UART konverterből nem volt bekötve
            //* legyen egy 4-es JST kimenet, I2C-vel, 3,3V-al és GND-al
            //* a hangrendszert a tápjának EN bemenetén keresztül lehessen ki-be kapcsolni
            //* piezo-t a PCB-re kell erősíteni
            //* az IR sensor fordítva van
            //* nem kell RGB LED
            //* befejezni 

    Marás előtt átnézni:
        //* mindennek van bemenete
        //* minden feszültségosztó jól van-e bekötve
        //* minden eszköz megfelelő tükrözésben van
            //? IR
            //? NPN
            //? ESP
            //? buck
            //? boost
            //? amplifier

        //* földelések
        //* vias, hole és trace méretek

** MARÁS UTÁN TALÁLT HIBÁK **
    //! az IR sensor kontaktja rossz oldalon volt
    //! a BMS board-on a bms P+ és P- pinjei közti távolság túl kicsi
    //! az I2C pinek rosszul lettek bekötve
    //! a sarkokat jelölő négyzet a coutout-ban belelógott a réztrack-ba
    

** NYOMTATÁS UTÁN TALÁLT HIBÁK **
    //! az elöl lévő két oldalsó kábelcsatorna közül az egyik nem kell
    //! az egyik kábelcsatorna belelóg a betétbe
    //! a terminal contact board boolBody-jának szélessége 85,3 a a contact board szélessége 86,7
    //! az akksi résznél ahol a connector PCB felfekszik ott nincs hely a forrasztási varratoknak a modelben hog belesüllyedjen
    //! a HEAD és COVER-t összetartó elülső két csavarja az elektronika résznél a HEAD-ben túl vékony a fal
    //! a mikrofon kontakt mágnes HEAD oldali boolBody-ja túl mély és nem ér ki a mágnes

ötletek az új verzióhoz
    //? a nagy reed relét a lámpához le kéne cserélni a kicsire és akkor egy jelző LED-et is lehetne mellé rakni

Hibák a tervezési folyamatban és dokumentációban
    // zavaró hogy amiket rajzokat készítettem nincsnek meg
    // sok modell amit creoban csináltam, csak stl-ben mentettem le


