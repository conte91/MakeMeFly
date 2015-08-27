#include <msp430.h>
#include <CBuf/cbuf.h>
#include "uart.h"

static char transmitting=0;
static ucharCircularBuffer uartTXBuffer, uartRXBuffer;

void initUart(unsigned int baud)
{
 UCA0CTL1 |= UCSWRST; /** Puts uart in reset to avoid problems */
 UCA0CTL0 = UCMODE_0; /* Uart mode */
 UCA0CTL1 = UCSSEL_2 | UCSWRST; /* SMCLK */
 UCA0BR0 = 130; // 16MHz 9600
 UCA0BR1 = 6; // 16MHz 9600
 UCA0MCTL = 0;
 UCA0CTL1 &= ~UCSWRST; /** Initialize USCI state machine */
}

void uartPut(unsigned char x){
  ucharPush(x, &uartTXBuffer);
  if(!transmitting){
    /* Start transmission */
    UCA0TXBUF=ucharPop(&uartTXBuffer);
    transmitting=1;
    UC0IE |= UCA0TXIE;
  }
}

char uartAvailable(unsigned char x){
  return ucharAvailable(&uartTXBuffer);
}
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
  if(ucharAvailable(&uartTXBuffer)){
    UCA0TXBUF=ucharPop(&uartTXBuffer);
  }
  else{
    UC0IE &= ~UCA0TXIE;
    transmitting=0;
  }
}
