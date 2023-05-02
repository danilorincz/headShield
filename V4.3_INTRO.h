Utolsó változtatások
    //* a HEAD oldalfalának megvastagítása a nyomtatási deformáció csökkentése érdekében
    //* összébb és lejjebb felljebb húzni a pánt csatlakozási pontját -> mégsem kell
    //* vagy a battery board PCB-n vagy a BATTERY CASE-en kell változtatni mert a forrasztási pont miatt nem fekszik fel megfelelően
    //* a BATTERY COVER-nek a tartókarjai nem kellenek egyátalán
    //* a HEAD-ben a contact pcb helyét kicsit meg kéne dönteni hogy könnyebben belesiklódjon a konnektor (2+2 fokkal lett megdöntve)
    //* szenzornak nincs elegendően nagy helye a COVER-en hogy bemenjen helyére rendesen (csavar fej ütközik) 
    //* a szenzornak kell egy tok
    //* Ellenőrizni az alátét átmérőjét amihez a lámpa mágnesesen csatlakozik
    
MARÁS (1. rendelés) hibák
    //* a resistorok lyukai körüli forrasztható gyűrű nagyon kicsi, valamint maga a lyuk is kicsi és belefolyik a festék
    //* az IR sensor lyukai túl kicsik ahhoz hogy a lábai tövénél lévő csavart rész is át tudjon menni
    //* a BATTERY BOARD DC005-ös adapter aljzatjának lyukai túl kicsik
    //* a battery boardnál a MB nincs bekötve
    //* a Co2 sensor új lett

design V4.3.1
A fő formai irányelvek megmaradnak, jelentős változtatás végrehajtására már nincs se szükség se idő. Azonban
felmerültek kissebb csatlakozási és tömítési problémák. Ezek kissebb módosításokat vonnak maguk után melyek nem
befojásolják a kinézetet.
    BATTERY
    //* a BATTERY nem megy be rendesen a helyére mert a két test átfedésben van
    //* a BATTERY-nél a mágnest tartó anyának kéne egy mélyedés
    //* a CONNECTOR_BOARD-ot helyesen kell pozícionálni
    COVER
    //* a COVER-en lévő tömítés lenyomó kiemelkedés a peremen ahol összeér a HEAD-el, túl alacsony és vékony
    VISOR
    //* a VISOR és a HEAD között túl nagy a légrés
    SENSOR_HOUSE
    //* a SENSOR_HOUSE-nál ahol a PCB becsúszik túl szűk
    //* más szenzor lesz más méretekkel

