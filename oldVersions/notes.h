//! MODELL
V7
> dubla levegőbeáramló készítése //*
> fejtartó betét helyének optimalizálása //*
> hátsó rész átméretezése //*
> a fedél feküdjön fel mindehol ahol csavar rogzíti //*
> szélvédő perem elkészítése //*
> a szenzor helyének módosítása, és konnektor lyuk nagyobbítása //*
> csavarbetétekhez készíteni egy próbanyomtatást //*

> szorosabbra csinálni a bayonett zárat //* (0.3 mm-el)
> egy nehezen elérhető mechanikus kapcsoló amivel áramtalanítani lehet //*
> szenzor csatlakozó túl kicsi, és a külsőhöz JST7-es kell nem JST6-os //*
> a fedél lyukait nagyobbra kel hagyni //*
> a betét és sisak csatlakozó illeszkedését tisztázni kell, és kicsitt hátrébb vinni
> a szélvédőnek nagyobb felfekvési terület kell, és több helyen kell ellentartani is
> kicsitt összébb kell húzni az archoz közelebb
V8
> a bayonett zárat kicsit közelebb kell vinni mert nagyon szorul rajta a szűrő
> a betét és a sisak illeszkedése
> a LED boolBody-t nagyobbra venni mert szorul

V9
A 2x2-es ventillátor összeállítás sikertelen. A következőkben az 5W-os radiális ventillátort fogom hazsnálni.

V10
Problémák az előzővel amit ki kell javítani:
> a fedelet tartó elülső két csavar boolBody-ja túl kicsi //*
> az elülső szenzor csatlakozót 180 fokkal meg kell fordítani //*
> az alaplapot túl alacsonyan van, ezért nem fér el alatta vezeték //*
> a szemellenzőnek vastagabb felfekvési területet kell biztosítani //*
> az IR szenzort feljebb kell hozni magához képest //*
> a rezgésérzékelőnek egy támasztást kell csinálni mert csak egy csavar tartja//*
> a sisaknak szűkebbnek kell lennie mert túl nagy a fejre //*
> a sisak hátradönthetőségének legyen két végállása //*
> a fedelet át kell alakítani hogy elférjenek benne az akkumulátorok//*

//! PROGRAM


//! TESZT NYOMTATÁS
- kell 5 szögben (0, 45, 90, 135, 180) mindenről egy modell
- kell 4 méret mindenből (5x4db modellt kell nyomtatni minden típusból)
- kell egy M3 és egy M4 anya
- kell egy M3 és egy M4 menetes betét
- kell egy JST konnektor
[5]Features: M3 nut / M4 nut / M3 screw / M4 screw / JST
[5]Angle: 0 / 45 / 90 / 135 / 180
[4]Size: -
5*5*4=100 db

V11
Jelentősen meg kell változtatni a modellt az alaplapot és ezáltal a programot is
Elektronika:
- ESP32
- Blower 5W
- piezoChannel
- RGB led
- tilt sensor
- ToF sensor

Modell
- szélvédő keretet el kell hagyni, helyette maga a szélvédő széle lesz a keret
- a szélvédőre mágnessel egy ruhapalyzsot kell tudni rakni
- egy levegőcsatornán keresztül kell hogy menjen a levegő az arc elé közvetlenül
