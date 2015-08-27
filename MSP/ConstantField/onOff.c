#include <msp430.h>
#include <stdint.h>
#include "uart.h"
#include <MSP_Dco/dco_custom_calibration.h>

#define PWM_TICK 16 /* 1MHz */
#define PWM_PERIOD 4000 /* 4 kHz PWM frequency */
#define FIFTY_PWM_PERIODS 400000
#define DESIRED_VOLTAGE (1.0)
#define TICK_PER_CONTROL 1
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
float BMax, BMin, B;
float BZero;
float growingRate, upRate, downRate;
float gainBase=1, gainPos=0.01, basePosition=300.0;
char coilOn, squareWave=0, outputUp=1, outputDown=1;
float tauUp, tauDown;
uint16_t lastADC;


static inline float BOf(float reading){
  return -(reading-BMin);
}
/*static inline float B(){
  return -(lastADC-BMin);
  }*/
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
  coilOn=value;
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
      if(!(i%8)) send(lastADC);
    }
    coil(0);
    for(int i=0; i<500; ++i){
      wait();
      if(!(i%8)) send(lastADC);
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
static inline uint16_t tareUp(){
  uint16_t tau;
  /** Ascending (charging) Tau */
  float valueWhichIHaveToSurpass=(BMax)*0.63;
  /** DESCENDING exp */
  coil(0);
  waitN(500);
  tau=0;
  coil(1);
  while(1){
    wait();
    if(B<valueWhichIHaveToSurpass){
      tau++;
      continue;
    }
    break;
    /** Ensure we really passed the point */
    wait();
    if(B>valueWhichIHaveToSurpass*0.95){
      /** Found it!! */
      break;
    }
    tau++;
  }
  return tau;
}

static inline uint16_t tareDown(){
  uint16_t tau;
  /** Descending (discharging) Tau */
  float valueWhichIHaveToSurpass=(BMax)*(1-0.63f);
  coil(1);
  waitN(500);
  tauDown=0;
  coil(0);
  while(1){
    wait();
    if(B>valueWhichIHaveToSurpass){
      tau++;
      continue;
    }
    break;
    /** Ensure we really passed the point */
    wait();
    if(B<valueWhichIHaveToSurpass*1.05){
      /** Found it!! */
      break;
    }
    tau++;
  }
  return tau;
}

char tareSensorValues(){
  /** Wait for the magnet to stabilize, then take the sensor reading */
  coil(0);
  iMustDoTheControl=0;
  waitN(1000);
  BMin=averageADC(100);
  coil(1);
  waitN(1000);
  BMax=averageADC(100);
  BMax=BOf(BMax);
  coil(0);
  waitN(1000);

  uint16_t sumUp=0, sumDown=0;
  for(int i=0; i<10; ++i){
    sumUp+=tareUp();
    sumDown+=tareDown();
  }
  tauUp=((float)sumUp-5.0)/10;
  tauDown=((float)sumDown-5.0)/10;

  upRate=1-1.0f/(1+1.0f/tauUp+1.0f/tauUp/tauUp/2.0f);

  downRate=-1+1/(1+1.0f/tauDown+1.0f/tauDown/tauDown/2.0f);

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

  ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
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
  tareSensorValues();
  /** Mimic the exponential to human-ensure it's ok */
  /** with multiplication */
  /*{
    float currentValue=1;
    for(int i=0; i<500; ++i){
    currentValue*=growingRate;
    if(!(i % 10))send((uint16_t)(currentValue*(BMin-BMax)+BMax));
    waitN(10);
    }
    }*/

  P1OUT &= ~BIT0;


  /** Start control */
  unsigned int i=0, j=0;
  coil(0);
  waitN(1000);
  float currentValue=0;
  while(1){
    wait();
    if(power < currentValue){
      coil(0);
    }
    else{
      coil(1);
    }
    /** Updates the hypothetical mag field */
    if(coilOn){
      currentValue+=(1-currentValue)*upRate;
    }
    else{
      currentValue+=currentValue*downRate;
    }
    if(currentValue>1){
      currentValue=1;
    }
    if(currentValue<0){
      currentValue=0;
    }

    if(++j==TICK_PER_CONTROL){
      j=0;
      i++;
      if(i==200){
        i=0;
      }
      P1OUT |= BIT0;
      float coilB=currentValue*BMax;
      float filteredValue=B-coilB;
      if(squareWave){
        if(i==0){
          coil(0);
        }
        if(i==100){
          coil(1);
        }
        if(!(i%8)){
          if((outputUp && coilOn) || (outputDown && !coilOn)){
            send(filteredValue);
          }
        }
      }
      else{
        float error=basePosition-filteredValue;
        float power=gainPos*error+gainBase;
        if(power < 0){
          power=0;
        }
        if(power > 1){
          power=1;
        }
        if(filteredValue < 20){
          power=0;
        }
        if(!(i%8)){
          if((outputUp && coilOn) || (outputDown && !coilOn)){
            send (power *256);
          }
        }
        
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
  B = -(lastADC-BMin);
  //countControl=0;
  iMustDoTheControl=1;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  char x=UCA0RXBUF;
  switch(x){
    case 'G':
      gainPos+=0.001;
      break;
    case 'g':
      gainPos-=0.001;
      break;
    case 'L':
      basePosition+=1;
      break;
    case 'l':
      basePosition-=1;
      break;
    case 'p':
      gainBase-=0.001;
      break;
    case 'P':
      gainBase+=0.001;
      break;
    case 's':
      squareWave=!squareWave;
      break;
    case 'd':
      downRate-=0.001;
      break;
    case 'D':
      downRate+=0.001;
      break;
    case 'u':
      upRate-=0.001;
      break;
    case 'U':
      upRate+=0.001;
      break;
    case 'o':
      outputDown=!outputDown;
      break;
    case 'O':
      outputUp=!outputUp;
      break;
  }
}

