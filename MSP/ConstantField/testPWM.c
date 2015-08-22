#include <msp430.h>
#include <ADC/adc.h>
#include <UART/uart.h>
#include <MSP_Dco/dco_custom_calibration.h>

#define PWM_TICK 16 /* 1MHz */
#define PWM_PERIOD 4000 /* 4 kHz PWM frequency */
#define CONTROL_CLOCK 40000 /* 400 Hz controller clock */
#define CONTROL_FREQ 10 /* 40 Hz controller freq */

/* We sample at 500Hz
 * 16Mhz clock
 * 9600 baud=1666tick/baud
 * 500Hz sample=32000 tick/sample
 *
 */

/* blinky blinky thing to avoid cdc_acm mess and other stuff like that */
void spettacolino(){
  volatile unsigned int i, k;
  P1OUT&=~BIT6;
  for(k=0; k<1; ++k){
    P1OUT|=BIT6;
    for(i=0; i<DCOFrequency>>17; ++i){
      __delay_cycles(65535);
    }
    P1OUT&=~BIT6;
    for(i=0; i<DCOFrequency>>17; ++i){
      __delay_cycles(65535);
    }
  }
}

int main(void)
{
  unsigned long j, k, i;
  WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */
  Setup_DCO(DCO_16MHZ);
  BCSCTL2=SELM_0 | DIVM_0 |  DIVS_0; /* MCLK=DCO, SMCLK=DCO */
  spettacolino();
  configureADC();
  __enable_interrupt();
  P1DIR|=BIT0|BIT2|BIT6;
  P1SEL |= BIT6; P1SEL2 &= ~BIT6; // P1.6 to TA0.1
  P1SEL |= BIT1|BIT2; P1SEL2 |= BIT1|BIT2; // UART pins
  initUart(9600);
  CCR0 = 4000;             // PWM frequency of 4kHz
  CCR1 = 0;
  CCTL1 = OUTMOD_7;          // CCR1 reset/set
  TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode
  while(1){
    for(j=0; j<=4000; j+=16){
      /** 100 periods at desired PWM freq */
      CCR1 = j;                // CCR1 PWM duty cycle
      __delay_cycles(200000);
      P1OUT|=BIT0;
      __delay_cycles(200000);
      P1OUT&=~BIT0;
      unsigned int currentSensorValue;
      if(!ADCAvailable()){
        while(!ADCAvailable());
      }
      while(ADCAvailable()){
        currentSensorValue=ADCnext();
      }
      uartPut(currentSensorValue&0x00FF);
      uartPut((currentSensorValue&0xFF00) >> 8);
    }
  }
  return 0;
}
