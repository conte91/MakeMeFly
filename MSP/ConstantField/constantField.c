#include <msp430.h>
#include <stdint.h>
#include <ADC/adc.h>
#include <UART/uart.h>
#include <MSP_Dco/dco_custom_calibration.h>

#define PWM_TICK 16 /* 1MHz */
#define PWM_PERIOD 4000 /* 4 kHz PWM frequency */
#define FIFTY_PWM_PERIODS 400000
#define CONTROL_FREQ 100 /* 40 Hz controller freq */
#define DESIRED_VOLTAGE (1.0)
#define K (-250.0)
#define KI -1.0 //(-1.0)
#define KD 0.1 // (-1.0)
#define MAX_ERROR_I 4000.0
#define MIN_ERROR_I -4000.0
#define alpha (0.95)

/* We sample at 500Hz
 * 16Mhz clock
 * 9600 baud=1666tick/baud
 * 500Hz sample=32000 tick/sample
 *
 */

int controlTimerN=0;
char iMustDoTheControl=0;
float minMagnetSensorValue, maxMagnetSensorValue;
float desiredLevitatingSensorValue;
float sensorMagnetSlope;
float minDrivingSensorValue;
float fastCubRoot(float f);

float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

unsigned int readLastADC(){
  unsigned int currentSensorValue;
  if(!ADCAvailable()){
    while(!ADCAvailable());
  }
  while(ADCAvailable()){
    currentSensorValue=ADCnext();
  }
  return currentSensorValue;
}

void heatUp(){
  /** Heat up? */
  uint16_t i;
  for(i=0; i<PWM_PERIOD; i+=(PWM_PERIOD>>8)){
    CCR1=i;
    __delay_cycles(FIFTY_PWM_PERIODS);
    uint16_t lastStuff=readLastADC();
    uartPut(lastStuff&0x00FF);
    uartPut((lastStuff>>8)&0x00FF);
  }
  CCR1=0;
  __delay_cycles(FIFTY_PWM_PERIODS);
}


void tareSensorValues(){
  /** Wait for the magnet to stabilize, then take the sensor reading */
  CCR1=PWM_PERIOD;
  __delay_cycles(FIFTY_PWM_PERIODS);
  __delay_cycles(FIFTY_PWM_PERIODS);
  __delay_cycles(FIFTY_PWM_PERIODS);
  __delay_cycles(FIFTY_PWM_PERIODS);
  maxMagnetSensorValue=readLastADC();
  /** And again, and again, and again */
  CCR1=0;
  __delay_cycles(FIFTY_PWM_PERIODS);
  __delay_cycles(FIFTY_PWM_PERIODS);
  __delay_cycles(FIFTY_PWM_PERIODS);
  __delay_cycles(FIFTY_PWM_PERIODS);
  minMagnetSensorValue=readLastADC();
  desiredLevitatingSensorValue=150;//minMagnetSensorValue/1.5;//fastCubRoot(minMagnetSensorValue/1.5); /** Remember that MIN field is MAX reading! We want about 1 volt to be stable */
  minDrivingSensorValue=minMagnetSensorValue*0.95;
  /** We approximate the sensor as linear in its region */
  sensorMagnetSlope=(maxMagnetSensorValue-minMagnetSensorValue)/PWM_PERIOD;
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

float fastCubRoot(float f){

  typedef union {
    float myfloat;
    uint32_t mybin;
  } floatBinary;

  uint32_t  fExp=0x7F800000;
  floatBinary f1, f2;
  //float result;
  float xn3;
  if(f==0.0f){
    return 0.0f;
  }

  f1.myfloat=f;
  char myExp=((f1.mybin&fExp) >> 23)-127;
  myExp/=3;
  uint32_t myExpPart=((uint32_t)(myExp+127))<<23;

  /** Compose the exponent again */
  f1.mybin = (myExpPart & fExp) | (f1.mybin & ~fExp);
  //xn3=f1.myfloat*f1.myfloat*f1.myfloat;
  //f1.myfloat=f1.myfloat*(xn3+2*f)/(2*xn3+f);
  xn3=f1.myfloat*f1.myfloat;
  f1.myfloat=(f/xn3+2*f1.myfloat)/3;
  xn3=f1.myfloat*f1.myfloat;
  f1.myfloat=(f/xn3+2*f1.myfloat)/3;
  xn3=f1.myfloat*f1.myfloat;
  f1.myfloat=(f/xn3+2*f1.myfloat)/3;
  //f2.myfloat=f2.myfloat*(f2.myfloat*f2.myfloat*f2.myfloat+2*b)/(2*

  return f1.myfloat;
}

int main(void)
{
  unsigned long j, k, i;
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
  configureADC();
  __enable_interrupt();
  P1DIR|=BIT0|BIT2|BIT6;
  P1SEL |= BIT6; P1SEL2 &= ~BIT6; // P1.6 to TA0.1
  P1SEL |= BIT1|BIT2; P1SEL2 |= BIT1|BIT2; // UART pins
  initUart(9600);
  CCR0 = PWM_PERIOD;             // PWM frequency of 4kHz
  CCR1 = 0;
  CCTL1 = OUTMOD_7;          // CCR1 reset/set
  TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode

  /** Tare upper and lower values for the sensor */
  P1OUT |= BIT0;
  heatUp();
  tareSensorValues();

  /** Stabilize */
  CCR1 = 0;
  currentCoilStrength=0;
  __delay_cycles(FIFTY_PWM_PERIODS);
  P1OUT &= ~BIT0;

  /** Start control */
  CCTL0 |= CCIE;
  while(1){
    if(iMustDoTheControl){
      P1OUT|=BIT0;
      /** Fight! */
      sensorValue=readLastADC();
      /** Eliminate the current value of the coil's magnetic field */
      sensorInfluenceDueToMagnet=sensorMagnetSlope*currentCoilStrength;
      sensorValue-=sensorInfluenceDueToMagnet;
      if(sensorValue<minDrivingSensorValue){
        /** Ok, now the sensor is "behaving" as the coil was turned off, only the floating thing is considered 
         * Can do a sort of control 
         * Simple P to start */
        error=desiredLevitatingSensorValue-sensorValue;//fastCubRoot(sensorValue);
        error=fastCubRoot(error);
        errorI+=error;
        if(errorI>MAX_ERROR_I){
          errorI=MAX_ERROR_I;
        }
        if(errorI<MIN_ERROR_I){
          errorI=MIN_ERROR_I;
        }
        errorD=error-prevError;
        prevError=error;
        impulse=error*K+errorI*KI+errorD*KD;
        impulse=alpha*impulse+(1-alpha)*currentCoilStrength;
        //impulse=Q_rsqrt(impulse);
        if(impulse<0){
          impulse=0;
        }
        if(impulse>PWM_PERIOD){
          impulse=PWM_PERIOD;
        }
        uImpulse=impulse;
        uCSV=uImpulse;
        uartPut(uCSV&0x00FF);
        uartPut((uCSV>>8)&0x00FF);
        CCR1=uImpulse;
        currentCoilStrength=impulse;
      }
      else{
        /** Shutdown */
        CCR1=0;
        currentCoilStrength=0;
      }
//      uCSV=sensorValue;
//      uartPut(uCSV&0x00FF);
//      uartPut((uCSV>>8)&0x00FF);
      P1OUT&=~BIT0;
      iMustDoTheControl=0;
    }
  }
  return 0;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
  controlTimerN++;
  if(controlTimerN==CONTROL_FREQ){
    controlTimerN=0;
    iMustDoTheControl=1;
  }
}
