#include <msp430.h>
#include <UART/uart.h>
#include <ADC/adc.h>
#include <MSP_Dco/dco_custom_calibration.h>

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

void spettacolino2(){
  volatile unsigned int i, k;
  P1OUT&=~BIT0;
  for(k=0; k<1; ++k){
    P1OUT|=BIT0;
    for(i=0; i<122; ++i){
      __delay_cycles(65535);
    }
    P1OUT&=~BIT0;
    for(i=0; i<122; ++i){
      __delay_cycles(65535);
    }
  }
}

void sendByte(unsigned char byte){
  int value;
  char send=0;
  P1OUT|=BIT0;

  /** Take only the recentmost value from ADC, if any */
  while(ADCAvailable()){
    send=1;
    value=ADCnext();
  }

  if(send){
    uartPut(value&0x00FF);
    uartPut((value&0xFF00) >> 8);
  }

  __bis_SR_register(LPM0_bits+GIE); /* Waits for timer to tick*/

  P1OUT&=~BIT0;

}

void setupTimer(){
  TACTL = TASSEL_2 | MC_0 | ID_3;           // SMCLK, reset
  TAR=0; /** Reset timer value */
  TACCR0 =  4000;                           // 4000 = 500Hz @ 16MHZ/8 (2MHZ)
  TACTL |= MC_1 ;
  TACCTL0= CCIE; /** Start up mode, enable interrupt */
}

int main(void)
{
  unsigned char i='A';
  WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */
  Setup_DCO(DCO_16MHZ);
  BCSCTL2=SELM_0 | DIVM_0 |  DIVS_0; /* MCLK=DCO, SMCLK=DCO */
  P1DIR = BIT0|BIT1|BIT6;
  P1OUT&=~BIT0;
  P1SEL |= BIT1|BIT2; /* Uart pin function */
  P1SEL2 |= BIT1|BIT2;

  __enable_interrupt();

  for(i=0;i<5;++i)
    spettacolino();

  initUart(9600);
  configureADC();
  setupTimer();

  while(1)
  {
      sendByte(i);
  }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMERA_ISR()
{
  __bic_SR_register_on_exit(LPM0_bits); /** Restart main */
}
