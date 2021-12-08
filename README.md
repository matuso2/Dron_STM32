# Náplň cvičenia
- zoznámenie sa s I2C
- komunikácia s IMU prostredníctvom I2C


# I2C zbernica

### Základné posielanie dát
<p align="center">
    <img src="https://github.com/VRS-Predmet/vrs_cvicenie_9/blob/master/docs/I2C_Basic_Address_and_Data_Frames.jpg" width="950" title="">
</p>

### Posielanie večšieho množstva dát
<p align="center">
    <img src="https://github.com/VRS-Predmet/vrs_cvicenie_9/blob/master/docs/I2C_Repeated_Start_Conditions.jpg" width="550" title="">
</p>

# Konfigurácia I2C

<p align="center">
    <img src="https://github.com/VRS-Predmet/vrs_cvicenie_9/blob/master/images/i2c_conf.PNG" width="850" title="GPIO pin block scheme">
</p>

- na obrázku je zobrazená konfigurácia I2C - okrem zobrazených parametrov je nutné povoliť globálne prerušenia od I2C ("error interrupt" nie je momentálne potrebný)
- MCU má rolu "master", číta z registrov alebo zapisuje do registrov senzora ("slave") 


# IKS01A1 senzorová doska

<p align="center">
    <img src="https://github.com/VRS-Predmet/vrs_cvicenie_9/blob/master/images/sensor_board.jpg" width="350" title="GPIO pin block scheme">
</p>

- obsahuje viacero senzorových jednotiek: magnetometer, IMU, sonzor vlhkosti ...
- v tomto prípade MCU komunikuje s IMU (acc a gyro) LSM6DS0
- na obrázku je znázornené zapojenie senzorovej dosky k vývojovej doske s MCU

- ukážkový program komunikuje s LSM6DS0 prostredníctvom I2C a číta hodnotu zrýchlení v osiach x, y, z

# Zadanie

Vytvoriť aplikáciu, ktorá bude zobrazovať dáta získané zo senzorov (LPS25HB, HTS) na 7 segmentovom displeji. Zobrazovanými údajmi budú teplota, relatívna vlhkosť vzduchu, tlak vzduchu a nadmorská výška.

### Úlohy

1. Vytvoriť si vlastnú knižnicu pre senzory LPS25HB a HTS221(podobne ako vyzerá knižnica pre IMU (acc + gyro) v demo ukážke). 

1a. Knižnica musí obsahovať inicializačnú funkciu, ktorej úlohou je overiť pripojenie senzora a vykonať počiatočnú konfiguráciu senzora (ekvivalent ku "lsm6ds0_init" z demo ukazky). Ako prvé overíte, či viete prečítať "WHO_AM_I" register a či hodnota, ktorú vám senzor vráti je totožná s hodnotou z dokumentácie. Následne zapíšete do registrov sonzora svoju vlastnú konfiguráciu. Prejdite si dokumentáciu, zistite čo všetko viete nastavovať pomocou registrov a podľa potreby si zvoľte vlastnú konfiguráciu senzora (frekvencia merania, merací rozsah ... ).

1b. Knižnica musí obsahovať funkciu na čítanie/zapisovanie dáť zo/do senzora. Zapisovanie do senzora bude slúžiť napr. pri konfigurácii senzora a čítanie bude slúžiť na získavanie aktuálneho stavu senzora (ak je to potrebné), hodnôt meraných veličín ... . Ak zo senzora budete čítať viac ako jednu veličinu (napr. teplota a vlhkosť), tak pre každú meranú veličinu vytvorte samostatnú funkciu na jej čítanie. V demo príklade je funkcia "lsm6ds0_get_acc" pre získanie zrýchlení. AK by sa vyčítavali aj uhlové rýchlosti z gyra, knižnica by obsahovala funkciu "lsm6ds0_get_gyro".

2. Údaje, ktoré sa majú zobrazovať ale nie su priamo získateľné zo senzora je potrebné na základe dostupných meraných hodnôt vypočítať (napr. nadmorská výška, ...). Pre takýto "post processing", kedy sa z meraných údajov snažite niečo vypočítať vytvorte samostatnú funkciu.

3. Údaje sa budú zobrazovať na 7-segmentovom dipleji tak, ako v predošlom zadaní. 

4. Formátovanie zobrazovaného textu. Hranatá zatvorka predstavuje v akých jednotkách je zobrazovaná hodnota. Niektoré hodnoty sú zobrazované s presnosťou na 1 alebo 2 desatinné miesta. "xx.x" predstavuje digity vyhradené pre číslice.:
   - teplota [°C]: "TEMP_xx.x"
   - rel. vlhkosť [%]: "HUM_xx"
   - tlak vzduchu [hPa]: "BAR_xxxx.xx"
   - nadmorská výška [m]: "ALT_xxxx.x"
   
   Príklad: ak je nameraná teplota 25.4°C, na dipleji sa bude zobrazovať text "TEMP_25.4". Ak by však bola teplota záporna, zobbrazovaný text bude "TEMP_-25.4". V prípade, že by nameraná hodnota bola +-100°C, zobrazený text bude obsahovať maximálnu/minimálnu hodnotu (podľa toho ktorá hranica sa prekoná) - "TEMP_99.9" alebo "TEMP_-99.9" .
   
5. To, ktorý z údajov sa bude aktuálne zobrazovať na displeji bude možné voliť pomocou tlačítka. To, ktorý údaj sa bude zobrazovať po resete je na Vás. Po stlačení tlačidla sa musí aplikácia prepnúť na zobrazovanie iného údaju (postupne je možné sa preklikať všetkými údajmi). Poradie v akom sa budú zobrazovať si zvolte podľa vlastného uváženia, ale musí byť fixné. Ak by bolo poradie zobrazovania údajov tak, ako je to v bode 4, po reštarte sa bude zobrazovať teplota. Po stlačení tlačidla sa prepne zobrazovanie na rel. vlhkosť, tlak a nakoniec nadmorskú výšku. Ak bude tlačidlo znovu stlačené, zobrazí sa teplota a celý cyklus sa bude opakovať pokiaľ bude stláčané tlačidlo. 

6. Zapojenie:
   - pripojenie senzorovej dosky je zobrazené na obrazku výššie, na MCU je SDA a SCL vyvedené na PB6 a PB7
   - tlačídlo bude pripojené na GPIO pin "PB3"
   - Segmenty displeja:   
      - segment A  -> PA1           
      - segment B  -> PA0             
      - segment C  -> PA8             
      - segment D  -> PB5             
      - segment E  -> PA11            
      - segment F  -> PA3
      - segment G  -> PB4
      - segment DP -> PB1

   - digity displeja:   
      - digit 0 ->    PB0
      - digit 1 ->    PA4
      - digit 2 ->    PA12
      - digit 3 ->    PA2
      - digit time -> PA7

