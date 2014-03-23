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
// Global variables
unsigned int ADC_value=0;

// Function prototypes
//void ConfigureAdc(void);

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
  /*
     volatile unsigned int toSend=(((unsigned int)byte)<<1)|0x01;
     volatile char count;

     P1OUT |= BIT0;

     for(count=9; count>=0; --count){
     if((toSend>>count) & 0x01){
     P1OUT|=BIT1;
     }
     else{
     P1OUT&=~BIT1;
     }
     __delay_cycles(155);
     }

     P1OUT &= ~BIT0;
     */
  
  int value;
  char send=0;
  P1OUT|=BIT0;

  while(ADCAvailable()){
    send=1;
    value=ADCnext();
  }

  if(send){
    uartPut(value&0x00FF);
    uartPut((value&0xFF00) >> 8);
  }

  __bis_SR_register(LPM0_bits+GIE); /* Waits for timer */

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
  WDTCTL = WDTPW + WDTHOLD;        // Stop WDT
  Setup_DCO(DCO_16MHZ);
  BCSCTL2=SELM_0 | DIVM_0 |  DIVS_0; /* MCLK=DCO, SMCLK=DCO */
  P1DIR = BIT0|BIT1|BIT6;
  P1OUT&=~BIT0;
  P1SEL |= BIT1|BIT2;
  P1SEL2 |= BIT1|BIT2;
  UCA0CTL1 |= UCSWRST; // **Initialize USCI state machine**
  __enable_interrupt();
  for(i=0;i<5;++i)
    spettacolino();
  //P1SEL |= BIT3;                    // ADC input pin P1.3
  //  __enable_interrupt();            // Enable interrupts.
  //
  //  __delay_cycles(1000);                // Wait for ADC Ref to settle

  initUart(9600);
  configureADC();                    // ADC set-up function call
  setupTimer();

  while(1)
  {
    for(i='0'; i<='9'; ++i){
      /*
         ADC10CTL0 |= ENC + ADC10SC;            // Sampling and conversion start
         __bis_SR_register(LPM0_bits + GIE);    // Low Power Mode 0 with interrupts enabled
         ADC_value = ADC10MEM;                // Assigns the value held in ADC10MEM to the integer called ADC_value
         */
      sendByte(i);

    }
  }
}
/*

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
__bic_SR_register_on_exit(LPM0_bits);        // Return to active mode }
}

// Function containing ADC set-up
void ConfigureAdc(void)
{

ADC10CTL1 = INCH_3 + ADC10DIV_3 ;         // Channel 3, ADC10CLK/3
ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
ADC10AE0 |= BIT3;                         // ADC input enable P1.3
}
*/

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMERA_ISR()
{
  __bic_SR_register_on_exit(LPM0_bits); /** Restart main */
}
