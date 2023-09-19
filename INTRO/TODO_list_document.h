//* Automatikusan működő html_to_string.py létrehozása

//* 1_Programozás
    //* szűrő idő számítás
        //* meghatározni az optimális frissítési időt
        //* algorítmust írni a memóriakezeléshez
        //* szűrő idő osztály létrehozni
    //* hibák
        //* akkumulátor töltöttség ne növekedhessen
    //* webes felület
        //* szűrő idő
        //* szűrő idő újraindítási lehetőség
        //* figyelmeztető rendszer kikapcsolás
        //* memória állapot százalék
        //* weblap szerkesztése a végleges fromájába
    //* figyelmeztető ciklus
    //* OTA
    //* lehessen változtatni az SSID-t
    // normal tartomány alsó és felső határának kitolása, úgy hogy épp hogy jelezzen a megváltozott körülményre

//* 2_Műszaki leírás
    //* kiegészíteni a szenzoros cuccokkal
        //* behivatkozni azt a táblázatot az ESN160 leírásából
    //* mégegyszer átfutni légszűrő rendszer mérését és kiegészíteni illetve javítani ha programban változás történt
    //* 4.5 Névleges fordulatszám meghatározása => kiegészítés
    //* 4.7 Mérési eredmények kezelése => szerksztés
    //* 4.11 Működés => annak magyarázata hogy miért nő a fordulatszám meghibásodott ventilátor esetén
    //* a "noFilter" és "faultFan" profilok egyesítve lettek, de itt még külön szerepel
    //* "unknown" és "malfunction" profilok kifejtése
    //* szűrő idő mérés kifejtése
    //* 4.4 Névleges térfogatáram megállapítása => szerkesztés
    //* figyelmeztető ciklus

//* 3_Használati útmutató
    //* webes felület kiegészítése a szűrő frissítéssel

// 4_Védőszövet
    // anyag választás
    // szabásminta
    // legyártani 7 db-t
    // dxf fájl készítése

//* 5_Vezérlőprogram leírásának frissítése a végleges program alapján

//* 6_Dokumentum hibajavítások
// Megoldani az anemométer szigetelését
// Mindegyik sisakon egy füstös vizsgálatot végezni
// Matricákat felragasztani
//* Feltölteni minden sisakra a legújabb kódot
// Minden sisak kapjon egyedi SSID-t
//* Tesztelni az akksit -> írni egy kódot ami elvégzi az elemzést
// Tesztelni az időmérőt -> megvizsgálni egy normál használat közbeni időt, miközben manuálisan is mérjük az időt
// Tesztelni a vezetéknélküli feltöltést
// Becsomagolni mindent

//! PROBLÉMÁK
// HS1: azakkumulátor töltöttségi szint mérése 83%-ot mutat amikor 100%-ot kéne
//* HS1: a reed kapcsoló nem érzékel megfelelően csak ha odébb nyomjuk a mágnest
//* HS4: nincs különbség a normal és noAir között






