V12 vagy V11 v2 vagy V3.2

Részek:
1. Head (fej) H
2. Grid (rács) G
3. Cover (fedő) C
4. Visor (szélvédő) V
5. Battery top (akksi felső) BT
6. Battery borrom (akksi alsó) BB
7. Slider left (bal csúszka) SR
8. Slider right (jobb csúszka) SL
9. Lamp left (bal lámpa test) LL
10. Lamp cover left (bal lámba fedő) LLC
11. Lamp right (jobb lámpa test) LR
12. Lamp cover right  (jobb lámpa fedő) LRC

Szükséges változtatások
        //* elülső pánt támasz eltávolítása
        //* vastagság növelése a homlokrésznél
        //* ventillátorok helyének növelése
        //* rács különválasztása
        //* pánt csatlakozó pontjának feljjebb helyezése
        //* LED helye
        //* visor tengely head felöli oldala nem hexa
        //* infra szenzor helye
        //* Head és Cover közötti tömítés
        //* akkumulátor csatlakozását újragondolni, és megerősíteni
        //* H és C rögzítése egymáshoz több kissebb csavarral legyen
        //* hangszóró
        //* erősítő
        //* mikrofon
        //* lámpa
        //* lámpa vezeték torkolat - kell hozzá tundi a vezeték átmérőjét
        //* mikrofon vezeték torkolat - kell hozzá tundi a vezeték átmérőjét
        //* kapcsoló méretezése - meg kell mérni a kapcsolót
        //* reed helye
        //* mikrofon vezeték rögzítése a visor-on
        //* dupla PCB - nagyjából meg kell csinálni a pcb-t
        //* hexa könnyítés

        > csusztató nyílás végállásolása
        > akkumulátor tartó

PCB V6
    Szenzorok/bemeneti cuccok:
        > Infra Red sensor
        > Reed Switch
        > Battery voltage

BOM
    mikrofon
        https://www.tme.eu/en/details/ld-mc-6035w-1/microphones-and-headsets/loudity/ld-mc-6035w/
        https://www.tme.eu/en/details/ld-mc-9765p/microphones-and-headsets/loudity/
        https://www.tme.eu/en/details/ld-mc-9765w/microphones-and-headsets/loudity/

    erősítő
        https://www.tme.eu/en/details/oky3462-3/audio-modules/okystar/

    hangszóró
        https://www.tme.eu/en/details/vs-frws-4-nd/speakers/visaton/2207/
        https://www.tme.eu/en/details/ld-sp-um20_8a/speakers/loudity/
        https://www.tme.eu/en/details/vs-sc5.9nd_8/speakers/visaton/8056/

    led
        https://www.tme.eu/en/details/ll-hp60mw1l-3w/white-power-leds-emiter/luckylight/
        https://www.tme.eu/en/details/ll-hp60mw1l-s/white-power-leds-emiter/luckylight/
        https://www.tme.eu/en/details/nte30180-w/white-power-leds-emiter/nte-electronics/
        https://www.tme.eu/en/details/ll-hp60mw1l-s/awhite-power-leds-emiter/luckylight/

V3.3
    Szükséges változtatások
        //? tudom hogy kell:
            //* a head és a visor nem simul egymáshoz tökéletesen
            //* a lámpa vezetékének üreg kell
            //* a cover miatt kicsit túl szűk az elektronikának
            //* az áramköri lapoknak kissebbeknek kell lennie
            //* az áramköri lapokat hátrébb kell vinni
            //* reed relé helye
            //* az audio erősítőnek helyet kell csinálni
            //* a lámpa ne mágneses legyen
            //* a rácsnak nagyobb lyukak kellenek
            //* akksi
            //* csatlakozót kell csinálni a szövetnek
            //* mágneses végállásolás
            //* reed mágnes nagyobb legyen
            //* a mikrofon vezetékének üreget kell csinálni
            //* anya helye a PCB-nek
            //* könnyítés

V3.4 szükséges változtatások:
    > a touch sensorokhoz berakni egy anyát úgy hogy a csavar belefusson
    > a lámpa vezetékének máshogy kell csinálni a csőalagutat
    > a PCB-k a forrasztás miatt nem mennnek be tökéletesen
    > a reed relay helye rossz volt
    > a mikrofon vezeték csőalagutat is optimalizálni kell
    > a cover-t tartó bk. csavarok feje nehezen megy be a helyére
    > 


PCB V7
    Hibák az előzőnél:
        > az RGB led-nek nem gnd kell hanem 3.3V
        > nem volt piezo
        > a GPIO0-t kell GND-ra kötni hogy boot-oljon
        > az IR-nak nem kell 4 pin csak 3
        > az NPN transistor simbóluma fordítva volt
        > az egyik reed relay bemenete nem képes belső ellelálláson keresztül felhúzni a jelet

Rendelés:
    cables 0.25 mm2
        piros: https://www.tme.eu/en/details/heat180sif-0.25rd/silicone-cables-single-core/lapp/0047104/ //?[20m]
        fekete: https://www.tme.eu/en/details/heat180sif-0.25bk/silicone-cables-single-core/lapp/0047001/ //?[20m]
        kék: https://www.tme.eu/en/details/heat180sif-0.25bl/silicone-cables-single-core/lapp/0047002/ //?[10m]
        barna: https://www.tme.eu/en/details/heat180sif-0.25br/silicone-cables-single-core/lapp/0047003/ //?[10m]
        zöld: https://www.tme.eu/en/details/heat180sif-0.25gr/silicone-cables-single-core/lapp/0047006/ //?[10m]
        szürke: https://www.tme.eu/en/details/heat180sif-0.25gy/silicone-cables-single-core/lapp/0047106/ //?[10m]
        sárga: https://www.tme.eu/en/details/heat180sif-0.25yl/silicone-cables-single-core/lapp/0047005/ //?[10m]

    connectors
        stciks: https://www.tme.eu/en/details/dg332j5.0-24p13/pluggable-terminal-blocks/degson-electronics/dg332j-5-0-24p-13-00a-h/ //?[10db]
        USB-C: https://www.tme.eu/en/details/2436-02/usb-ieee1394-connectors/lumberg/2436-02/ //?[2db]

        TGB-2-female: https://www.tme.eu/en/details/tbw-5-2p_gn/pluggable-terminal-blocks/ninigi/ //?[30db]
        TGB-3-female: https://www.tme.eu/en/details/tbw-5-3p_gn/pluggable-terminal-blocks/ninigi/ //?[30db]
        TGB-4-female: https://www.tme.eu/en/details/tbw-5-4p_gn/pluggable-terminal-blocks/ninigi/ //?[30db]
        TGB-5-female: https://www.tme.eu/en/details/tbw-5-5p_gn/pluggable-terminal-blocks/ninigi/ //?[30db]
        TGB-6-female: https://www.tme.eu/en/details/tbw-5-6p_gn/pluggable-terminal-blocks/ninigi/ //?[30db]

        TGB-2-male: https://www.tme.eu/en/details/tbg-5-pw-2p-gn/pluggable-terminal-blocks/xinya/xy2500v-d-5-08-2pin/ //?[30db]
        TGB-3-male: https://www.tme.eu/en/details/tbg-5-pw-3p-gn/pluggable-terminal-blocks/xinya/xy2500v-d-5-08-3pin/ //?[30db]
        TGB-4-male: https://www.tme.eu/en/details/tbg-5-pw-4p-gn/pluggable-terminal-blocks/xinya/xy2500v-d-5-08-4pin/ //?[30db]
        TGB-5-male: https://www.tme.eu/en/details/tbg-5-pw-5p-gn/pluggable-terminal-blocks/xinya/xy2500v-d-5-08-5pin/ //?[30db]
        TGB-6-male: https://www.tme.eu/en/details/tbg-5-pw-6p-gn/pluggable-terminal-blocks/xinya/xy2500v-d-5-08-6pin/ //?[30db]
        JST-2-male-90: https://www.tme.eu/en/details/s2b-xh-a-1/raster-signal-connectors-2-50mm/jst/s2b-xh-a-1-lf-sn/ //?[30db]
        JST-3-male-90: https://www.tme.eu/en/details/s3b-xh-a-1/raster-signal-connectors-2-50mm/jst/s3b-xh-a-1-lf-sn/ //?[30db]
        JST-4-male-90: https://www.tme.eu/en/details/s4b-xh-a-1/raster-signal-connectors-2-50mm/jst/s4b-xh-a-1-lf-sn/ //?[30db]

    transistors
        NPN transistor: https://www.tme.eu/en/details/nte2343/npn-tht-darlington-transistors/nte-electronics/ //?[5db]
        n-ch mosfet: https://www.tme.eu/en/details/nte2388/tht-n-channel-transistors/nte-electronics/ //?[5db]
        
    audio
        amplifier: https://www.tme.eu/en/details/oky3462-3/audio-modules/okystar/ //?[3db]
        hangszóró: https://www.tme.eu/en/details/vs-frws-4-nd/speakers/visaton/2207/ //?[3db]
        mikrofon: https://www.tme.eu/en/details/ld-mc-9765p/microphones-and-headsets/loudity/ //?[3db]
    LED
        led: https://www.tme.eu/en/details/nte30180-w/white-power-leds-emiter/nte-electronics/ //?[10db]
    battery    
        cell: https://www.tme.eu/en/details/kp-imr26650-hd/rechargeable-batteries/keeppower/uh2655/ //?[4db]
