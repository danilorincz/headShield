DESIGN V3.6
A tervek szerint ebbe a verzióba csak a modelt csinálom újra 
az elektronika nagy részét átrakom az előző verzióból ebbe a verzióba.
Csak a BATTERY_PCB-t és a TERMINALPCB_MALE valamint ha szükséges akkor 
a TERMINALPCB_FEMAL-t kell újramarni csak.

    Szükséges változtatások
        DESIGN
            //* a COVER vastag részeinek kikönnyítése
            //* PERK csatlakozása a COVER-en a(z) USB-A_BOARD-hoz
            //* COVER felső végállásolása
            //* nagyobb csatlakozó alátét a LAMP-nek
            //* a mikrofon mágnesének boolBody-ja a COVER-ben túl mély és nem ér ki a mágnes
            //* a lámpa kábelcsatornája belelóg a betét boolBody-jába
            //* a HEAD elülső két HEAD -et és COVER-t összekötő csavarjánál túl vékony a fal
            //* a belső elöl lévő három kábelcsatornát rendezni
            //* könnyítések

        BATTERY
            //* BATTERY más formaterv
            // *a CONTACTPCB_MALE (BATTERY oldal) forrasztási farratainak nincs helye belesüllyedni
            //* rendezni a CONTACTPCB_MALE és CONTACTPCB_FEMALE méreteit és orientációit
    
    HIBÁK modell
            //! a BATTERY-nél ahol a CONTACT_PCB vezetékei kifutnak lyuk túl fent van
            //! a BATTERY-nél a CONTACT_BOARD forrasztási pontjai nem férnek el
            //! a BATTERY-nél nincs semmi ami meggátolná hogy az akksik kicsússzanak
            //! a BETTERY-nél a BATTERY_BOARD-nak nincs elég helye
            //! a lamp vezetékcsatornája fellülről kéne hogy belefusson
            //! a BATTERY-nek nem kell két oldalsó hármas terminal block
    HIBÁK marás
            //! a JST horizontal-oknak nagyobb drill kell
            //! a BATTERY_BOARD-on lévő terminal socket drill lyuknagysága túl kicsi
            //! a transistorok drill-jei túl kicsik


            //! a BATTERY_BOARD USB-C foglalatának adatlábainak nincs külön track
            //! a POWER_BOARD egyik kapacitása nincs bekötve csak ott van
            //! az I2C kimenet GND-je nem kapott kapcsolatot
            //! a resistorok forrasztásakor megolvadt az ESP foglalata mert túl közel van hozzá



