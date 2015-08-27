#include <msp430.h>
#include <stdint.h>
#include "uart.h"
#include <MSP_Dco/dco_custom_calibration.h>

#define PWM_TICK 16 /* 1MHz */
#define PWM_PERIOD 4000 /* 4 kHz PWM frequency */
#define FIFTY_PWM_PERIODS 400000
#define DESIRED_VOLTAGE (1.0)
#define TICK_PER_CONTROL 3
#define E (2.71828182845905)

/* We sample at 500Hz
 * 16Mhz clock
 * 9600 baud=1666tick/baud
 * 500Hz sample=32000 tick/sample
 *
 */

int controlTimerN=0;
char iMustDoTheControl=0, countControl=0;
float minMagnetSensorValue, maxMagnetSensorValue;
float desiredLevitatingSensorValue;
float sensorMagnetSlope;
float minDrivingSensorValue;
float fastCubRoot(float f);
float BMax, BMin;
float BZero;
float growingRate;
uint16_t tau;
uint16_t lastADC;


static inline float BOf(uint16_t reading){
  return -(reading-BMin);
}
#if 0
  float Q_rsqrt( float number )
  {
    int32_t i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( int32_t * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
  }


  float sqr( float x ) { return x * x; }
  // meaning of 'precision': the returned answer should be base^x, where
  //                         x is in [power-precision/2,power+precision/2]
  float mypow( float base, float power, float precision )
  {   
    char invert;
    if(power < 0){
      invert=1;
      power=-power;
    }
    float result=1;
    while(power >= 10){
      result*=result;
      power/=2;
      precision/=2;
    }
    while(power >= 1){
      power-=1;
      result*=base;
    }

    /*
     if ( power < 0 ) return 1 / mypow( base, -power, precision );
     */

    /*
     if ( power >= 10 ) return sqr( mypow( base, power/2, precision/2 ) );
     */
     /*
     if ( power >= 1 ) return base * mypow( base, power-1, precision );
     */
     if ( precision >= 1 ) return 1/Q_rsqrt( base );
     return 1/Q_rsqrt( mypow( base, power*2, precision*2 ) );



     if(invert){
       result=1/result;
     }
     return result;
  }

  float power( float base, float p ) { return mypow( base, p, .00001 );}
#endif
static inline void wait(){
  while(!iMustDoTheControl);
  iMustDoTheControl=0;
}
static inline void waitN(const int n){
  for(int i=0; i<n; ++i){
    wait();
  }
}

static inline void coil(const int value){
  P1OUT = value ? P1OUT | BIT6 : P1OUT & ~BIT6;
}

static inline void send(const uint16_t value){
  uartPut(value & 0xFF);
  uartPut((value >> 8) & 0xFF);
}

void heatUp(){
  /** Heat up? */
  for(int i=0; i<5; ++i){
    coil(1);
    for(int i=0; i<500; ++i){
      wait();
      if(i%10) send(lastADC);
    }
    coil(0);
    for(int i=0; i<500; ++i){
      wait();
      if(i%10) send(lastADC);
    }
  }
}


static inline float averageADC(const int n){
  float avg=0;
  for(int i=1; i<=n; ++i){
    wait();
    avg+=(lastADC-avg)/i;
  }
  return avg;
}
char tareSensorValues(){
  /** Wait for the magnet to stabilize, then take the sensor reading */
  coil(0);
  iMustDoTheControl=0;
  waitN(500);
  BMin=averageADC(100);
  coil(1);
  waitN(500);
  BMax=averageADC(100);
  coil(0);
  waitN(500);

  float valueWhichIHaveToSurpass=(BMin-BMax)*0.37+BMax;
  /** DESCENDING exp */
  while(lastADC>BMin*1.05 || lastADC<valueWhichIHaveToSurpass );
  wait();
  tau=0;
  coil(1);
  while(1){
    while(lastADC>valueWhichIHaveToSurpass){
      tau++;
      wait();
    }
    /** Ensure we really passed the point */
    wait();
    if(lastADC<valueWhichIHaveToSurpass*1.05){
      /** Found it!! */
      break;
    }
    tau++;
  }
  coil(0);
  waitN(500);
  growingRate=0.931062779704023;

  return 1;
}

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

void setupADC(){
  TACCR0 = PWM_PERIOD;             // PWM frequency of 4kHz
  TACCR1 = PWM_PERIOD/2;
  TACCTL1 = OUTMOD_7; /* No output */
  TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode

  /** Enable ADC triggering on timer event */
  P1DIR&=~BIT3;
  ADC10CTL1 = INCH_3 + SHS_1 + CONSEQ_2 + ADC10DIV_4 + ADC10SSEL_3;         // Channel 3, Trigger on TA0.0 output rising edge, source is SMCLK/8 (->256/op)

  ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10SR + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
  ADC10AE0 |= BIT3;                         // ADC input enable P1.3
  ADC10CTL0 |= ENC; /** Fight! */
  //TACCTL1 = OUTMOD_7; /* No output */

}

int main(void)
{
  float sensorValue;
  unsigned int currentCoilStrength;
  unsigned int uImpulse, uCSV;
  float sensorInfluenceDueToMagnet;
  float error, impulse;
  float errorI=0, errorD, prevError=0;
  WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */
  Setup_DCO(DCO_16MHZ);
  BCSCTL2=SELM_0 | DIVM_0 |  DIVS_0; /* MCLK=DCO, SMCLK=DCO */
  spettacolino();
  __enable_interrupt();
  P1DIR|=BIT0|BIT2|BIT6;
  P1SEL |= BIT1|BIT2; P1SEL2 |= BIT1|BIT2; // UART pins
  initUart(9600);
  setupADC();

  /** Tare upper and lower values for the sensor */
  P1OUT |= BIT0;
  heatUp();
  coil(0);
  for(int i=0; i<500; ++i){
    wait();
    if(!(i%10)) send(lastADC);
  }
  while(!tareSensorValues());
  /** Mimic the exponential to human-ensure it's ok */
  /*for(int i=0; i<500; ++i){
    float f=BMax+(BMin-BMax)*power(E,-((float)i)/tau);
    if(i%10)send((uint16_t)f);
    waitN(10);
  }*/
  /** Now with multiplication */
  {
    float currentValue=1;
    for(int i=0; i<500; ++i){
      currentValue*=growingRate;
      if(!(i % 10))send((uint16_t)(currentValue*(BMin-BMax)+BMax));
      waitN(10);
    }
  }

  P1OUT &= ~BIT0;


  /** Start control */
  unsigned int i=0, j=0;
  char coilOn=0;
  float currentValue=1, startB=BMin;
  while(1){
    wait();
    if(++j==TICK_PER_CONTROL){
      j=0;
    P1OUT |= BIT0;
    i++;
    if(currentValue>1e-4){
      currentValue=currentValue*growingRate;
    }
    float currentB=currentValue*(startB-(coilOn ? BMax : BMin))+(coilOn ? BMax : BMin);
    if(currentB>BMin){
      currentB=BMin;
    }
    if(currentB<BMax){
      currentB=BMax;
    }
    float filteredValue=lastADC-currentB;

#if 0
    if((filteredValue > -345 && (!coilOn)) || (filteredValue < -355 && coilOn)){
      currentValue=1;
      startB=currentB;
      if(coilOn){
        coil(0);
        coilOn=0;
      }
      else{
        coil(1);
        coilOn=1;
      }
    }
#else
    if(i==100){
      i=0;
      currentValue=1;
      startB=currentB;
      if(coilOn){
        coil(0);
        coilOn=0;
      }
      else{
        coil(1);
        coilOn=1;
      }
    }
#endif
    if(!(i%8)){
      send(filteredValue);
    }
    P1OUT &= ~BIT0;
    }

  }

  return 0;
}

/* ADC10 interrupt service routine */
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    lastADC = ADC10MEM & 0x03FF;                // Assigns the value held in ADC10MEM to the integer called ADC_value
    countControl=0;
    iMustDoTheControl=1;
}
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  if(UCA0RXBUF=='G') {
    growingRate+=0.01;
  }
  else if(UCA0RXBUF=='g'){
    growingRate-=0.01;
  }

}

