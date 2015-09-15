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
Sheet 5 5
Title ""
Date "7 sep 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 4250 3650 0    60   UnSpc ~ 0
IN
$Comp
L SW_PUSH SW501
U 1 1 55ED8E2C
P 4850 3250
F 0 "SW501" H 5000 3360 50  0000 C CNN
F 1 "SW_PUSH" H 4850 3170 50  0000 C CNN
F 2 "~" H 4850 3250 60  0000 C CNN
F 3 "~" H 4850 3250 60  0000 C CNN
	1    4850 3250
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW502
U 1 1 55ED8E3B
P 4850 3650
F 0 "SW502" H 5000 3760 50  0000 C CNN
F 1 "SW_PUSH" H 4850 3570 50  0000 C CNN
F 2 "~" H 4850 3650 60  0000 C CNN
F 3 "~" H 4850 3650 60  0000 C CNN
	1    4850 3650
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW503
U 1 1 55ED8E4A
P 4850 4050
F 0 "SW503" H 5000 4160 50  0000 C CNN
F 1 "SW_PUSH" H 4850 3970 50  0000 C CNN
F 2 "~" H 4850 4050 60  0000 C CNN
F 3 "~" H 4850 4050 60  0000 C CNN
	1    4850 4050
	1    0    0    -1  
$EndComp
Text HLabel 5150 3250 2    60   UnSpc ~ 0
BTN1
Text HLabel 5150 3650 2    60   UnSpc ~ 0
BTN2
Text HLabel 5150 4050 2    60   UnSpc ~ 0
BTN3
Wire Wire Line
	4250 3650 4550 3650
Wire Wire Line
	4550 3250 4550 4050
Connection ~ 4550 3650
$EndSCHEMATC
