EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:diodes
LIBS:regulators
LIBS:misc_components
LIBS:boosterpack
LIBS:maglev-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
Title ""
Date "15 sep 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L LM395T Q301
U 1 1 55ED7DED
P 3650 3150
F 0 "Q301" H 3595 2990 40  0000 R CNN
F 1 "LM395T" H 3650 3300 40  0000 R CNN
F 2 "~" H 3650 3150 60  0000 C CNN
F 3 "~" H 3650 3150 60  0000 C CNN
	1    3650 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 3150 3400 3150
$Comp
L INDUCTOR L301
U 1 1 55ED7DF5
P 3750 2650
F 0 "L301" V 3700 2650 40  0000 C CNN
F 1 "INDUCTOR" V 3850 2650 40  0000 C CNN
F 2 "~" H 3750 2650 60  0000 C CNN
F 3 "~" H 3750 2650 60  0000 C CNN
	1    3750 2650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR029
U 1 1 55ED7DFC
P 3750 3700
F 0 "#PWR029" H 3750 3700 30  0001 C CNN
F 1 "GND" H 3750 3630 30  0001 C CNN
F 2 "" H 3750 3700 60  0000 C CNN
F 3 "" H 3750 3700 60  0000 C CNN
	1    3750 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 3700 3750 3350
$Comp
L LM318N U302
U 1 1 55ED7E08
P 6700 2550
F 0 "U302" H 6800 2850 60  0000 C CNN
F 1 "LM318N" H 6910 2300 60  0000 C CNN
F 2 "" H 6700 2550 60  0000 C CNN
F 3 "" H 6700 2550 60  0000 C CNN
	1    6700 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 2250 6200 2450
Wire Wire Line
	7200 2550 7450 2550
$Comp
L R R302
U 1 1 55ED7E12
P 7450 3300
F 0 "R302" V 7530 3300 40  0000 C CNN
F 1 "1k" V 7457 3301 40  0000 C CNN
F 2 "~" V 7380 3300 30  0000 C CNN
F 3 "~" H 7450 3300 30  0000 C CNN
	1    7450 3300
	1    0    0    -1  
$EndComp
$Comp
L R R303
U 1 1 55ED7E18
P 7450 3800
F 0 "R303" V 7530 3800 40  0000 C CNN
F 1 "10k" V 7457 3801 40  0000 C CNN
F 2 "~" V 7380 3800 30  0000 C CNN
F 3 "~" H 7450 3800 30  0000 C CNN
	1    7450 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR030
U 1 1 55ED7E1E
P 7450 4050
F 0 "#PWR030" H 7450 4050 30  0001 C CNN
F 1 "GND" H 7450 3980 30  0001 C CNN
F 2 "" H 7450 4050 60  0000 C CNN
F 3 "" H 7450 4050 60  0000 C CNN
	1    7450 4050
	1    0    0    -1  
$EndComp
Connection ~ 7450 4050
Connection ~ 7450 3550
Connection ~ 7450 3050
Connection ~ 7200 2550
Connection ~ 6200 2450
Connection ~ 6200 2650
$Comp
L R R301
U 1 1 55ED7E2A
P 7450 2800
F 0 "R301" V 7530 2800 40  0000 C CNN
F 1 "1k" V 7457 2801 40  0000 C CNN
F 2 "~" V 7380 2800 30  0000 C CNN
F 3 "~" H 7450 2800 30  0000 C CNN
	1    7450 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 2650 6200 3550
Wire Wire Line
	6200 3550 7450 3550
$Comp
L GND #PWR031
U 1 1 55ED7E39
P 4500 2650
F 0 "#PWR031" H 4500 2650 30  0001 C CNN
F 1 "GND" H 4500 2580 30  0001 C CNN
F 2 "" H 4500 2650 60  0000 C CNN
F 3 "" H 4500 2650 60  0000 C CNN
	1    4500 2650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6200 2250 4200 2250
$Comp
L SS495 U301
U 1 1 55ED7E32
P 4250 2650
F 0 "U301" H 4400 2454 60  0000 C CNN
F 1 "SS495" H 4250 2850 60  0000 C CNN
F 2 "~" H 4250 2650 60  0000 C CNN
F 3 "~" H 4250 2650 60  0000 C CNN
	1    4250 2650
	0    -1   -1   0   
$EndComp
Text HLabel 3750 2350 1    60   Input ~ 0
12V
Text HLabel 4200 3050 3    60   Input ~ 0
5V
Text HLabel 6600 1900 1    60   Input ~ 0
12V
Text HLabel 6600 3150 3    60   Input ~ 0
-12V
$Comp
L R R304
U 1 1 55ED84CD
P 7700 2550
F 0 "R304" V 7780 2550 40  0000 C CNN
F 1 "100" V 7707 2551 40  0000 C CNN
F 2 "~" V 7630 2550 30  0000 C CNN
F 3 "~" H 7700 2550 30  0000 C CNN
	1    7700 2550
	0    -1   -1   0   
$EndComp
$Comp
L ZENER D302
U 1 1 55ED84DC
P 7950 2750
F 0 "D302" H 7950 2850 50  0000 C CNN
F 1 "1N5226B" H 7950 2650 40  0000 C CNN
F 2 "~" H 7950 2750 60  0000 C CNN
F 3 "~" H 7950 2750 60  0000 C CNN
	1    7950 2750
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR032
U 1 1 55ED84FE
P 7950 2950
F 0 "#PWR032" H 7950 2950 30  0001 C CNN
F 1 "GND" H 7950 2880 30  0001 C CNN
F 2 "" H 7950 2950 60  0000 C CNN
F 3 "" H 7950 2950 60  0000 C CNN
	1    7950 2950
	1    0    0    -1  
$EndComp
Text HLabel 7950 2550 1    60   Output ~ 0
B_SENSE
$Comp
L DIODE D301
U 1 1 55ED8B7F
P 3350 2650
F 0 "D301" H 3350 2750 40  0000 C CNN
F 1 "1N4007" H 3350 2550 40  0000 C CNN
F 2 "~" H 3350 2650 60  0000 C CNN
F 3 "~" H 3350 2650 60  0000 C CNN
	1    3350 2650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3350 2450 3550 2350
Wire Wire Line
	3550 2350 3750 2350
Wire Wire Line
	3750 2950 3550 2950
Wire Wire Line
	3550 2950 3350 2850
Text HLabel 3100 3150 0    60   Input ~ 0
COIL CONTROL
Connection ~ 6600 2150
Connection ~ 7950 2550
Connection ~ 7950 2950
Wire Wire Line
	6600 2950 6600 3150
Wire Wire Line
	6600 1900 6600 2150
$EndSCHEMATC
