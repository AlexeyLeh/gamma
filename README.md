# Gamma Detector Calibration

Compatible with BTO and COM ports.

### Loading Data from the Last Test

The system allows you to reload data from the most recent test.

### Simulation

A simulation mode is available, enabling you to verify algorithm performance during linearity and API tests.

It is activated via the .ini file:

```
[General]
Simulation=true
```

Two options will appear in the "File" menu:

* Load simulation
* Save simulation

A "Select simulation" button will appear in the linearity test interface.
Once a file is selected, it will be loaded, and the corresponding test graphs and data will be displayed.
Sample simulation files are located in the `Simulations` folder.

If the `Simulations` folder is missing, you can create a sample file using the "Save simulation" option.
This saves the test data hardcoded into the program.
The file can subsequently be edited to contain custom data using any text editor. Sample file:

```
Calibration Date: 07.10.2026 14:29
Tool P/N: 77557ES000
Tool S/N: SIM000000123
Operator: OTC
Tool Temperature: 29.3 / 88.2
Operating HV: 1425 

# HV CPS COLD CPS HOT
-------------------------------------- 
1 850 31.40 17.80 
2 875 47.00 22.80 
3 900 71.20 35.40 
4 925 86.40 49.60 
5 950 107.60 59.80 
6 975 146.80 72.60 
7 1000 174.20 91.60 
8 1025 214.80 144.20 
9 1050 250.20 178.20 
10 1075 270.60 210.60 
11 1100 291.00 233.60 
12 1125 309.20 260.00 
13 1150 310.80 276.80 
14 1175 311.00 294.80 
15 1200 312.00 293.00 
16 1225 317.20 313.60
17 1250 324.80 314.20
18 1275 326.20 324.80
19 1300 330.00 314.20
20 1325 330.80 315.00
21 1350 325.60 320.60
22 1375 330.80 331.20
23 1400 331.40 318.80
24 1425 307.80 315.20
25 1450 332.40 333.00
26 1475 315.40 326.40
27 1500 336.20 330.00
28 1525 317.20 331.20
29 1550 331.40 325.40
30 1575 338.80 338.80
31 1600 353.60 332.20
32 1625 333.60 352.00
33 1650 345.20 345.40
34 1675 345.60 349.20
35 1700 361.40 356.80
36     1725   363.40   380.80
37     1750   368.20   372.40
```

In the API test, the simulation is started by clicking the "Start API Test" button.
The test data is hardcoded.



### File naming convention:

gamma-bto-3.2.2-bin-win11-64-20260914-1145.7z

* gamma_bto - version supporting BTO and COM port (selectable in settings)
* version    - 3.2.2
* bin        - executable package
* src        - source code (Qt project)
* build      - win7-32 ​​or win11-64
* suffix     - build date and time