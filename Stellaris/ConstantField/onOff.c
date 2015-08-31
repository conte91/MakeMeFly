#include <inc/hw_memmap.h> 
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <driverlib/debug.h>
#include <driverlib/sysctl.h> 
#include <driverlib/adc.h> 
#include <driverlib/interrupt.h>
#include <driverlib/uart.h>
#include <driverlib/gpio.h> 
#include <driverlib/timer.h>

#include "variables.h"
#ifdef DEBUG //Error Checking on APIs
void__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif
#define ADC_FREQUENCY 10000
#define TICK_PER_CONTROL 1
#define TICK_PER_SEND 8

char coilOn;
float tauUp, tauDown;

static inline void led(const int value){
  if(value){
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
  }
  else{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
  }
  coilOn=value;
}
static inline void coil(const int value){
  if(value){
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
  }
  else{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
  }
  coilOn=value;
}

static inline void wait(){
  while(!iMustDoTheControl);
  iMustDoTheControl=0;
}
static inline void send(const uint16_t value){
  UARTCharPutNonBlocking(UART0_BASE, value & 0xFF);
  UARTCharPutNonBlocking(UART0_BASE, (value >> 8) & 0xFF);
}
static inline float BOf(float reading){
  return -(reading-BMin);
}

void heatUp(){
  /** Heat up? */
  for(int i=0; i<5; ++i){
    coil(1);
    for(int i=0; i<ADC_FREQUENCY; ++i){
      wait();
      if(!(i%TICK_PER_SEND)) send(lastADC);
    }
    coil(0);
    for(int i=0; i<ADC_FREQUENCY; ++i){
      wait();
      if(!(i%TICK_PER_SEND)) send(lastADC);
    }
  }
}

void spettacolino(){
  for(int i=0; i<10; ++i){
    SysCtlDelay(SysCtlClockGet()/4/3);
    led(1);
    SysCtlDelay(SysCtlClockGet()/4/3);
    led(0);
  }
}
static inline void waitN(const unsigned int n){
  for(unsigned int i=0; i<n; ++i){
    wait();
  }
}

void setupADC(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //Enable ADC0 Peripheral
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //Enable Port A Peripheral
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerEnable(TIMER0_BASE, TIMER_A);
  SysCtlADCSpeedSet(SYSCTL_ADCSPEED_250KSPS); //ADC Sample Rate set to 250 Kilo Samples Per Second
  unsigned long timerPeriod=SysCtlClockGet() / ADC_FREQUENCY;
  TimerLoadSet(TIMER0_BASE, TIMER_A, timerPeriod-1);

  ADCHardwareOversampleConfigure(ADC0_BASE, 16); // Hardware averaging. ( 2, 4, 8 , 16, 32, 64 )
  //64 Samples are averaged here i.e, each sample will be a result of 64 averaged samples. Therefore, every result is a result of 64 x 4 = 256 samples.

  ADCSequenceDisable(ADC0_BASE, 0); //Before Configuring ADC Sequencer 1, it should be OFF

  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_TIMER, 0);
  //ADC Configured so that Processor Triggers the sequence and we want to use highest priority. ADC Sequencer 0 is Used.

  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0|ADC_CTL_IE|ADC_CTL_END); //Sequencer Step 0: Samples Channel PE3
  //ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
  //Final Sequencer Step also Samples and enables Interrupt and we are telling the sequencer that this is the last step

  //Configuring all eight steps in the ADC Sequence
  ADCSequenceEnable(ADC0_BASE, 0); //Enable ADC Sequence

  TimerEnable(TIMER0_BASE, TIMER_A);
  TimerControlTrigger(TIMER0_BASE,TIMER_A,1); // NOTE: Specifically for ADC!!!!!!
  ADCIntClear(ADC0_BASE, 0);     // Clear any outstanding Interrupts
  ADCIntEnable(ADC0_BASE, 0); //ALLOW --  start of the collection ChainADCIntEnable(ADC0_BASE,0);
  IntEnable(INT_ADC0SS0); 
}

void setupUART(){
  /** Configure 1MBaud UART */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //Enable Port F Peripheral
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); 
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 1000000, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); 
  UARTFIFOEnable(UART0_BASE);
  UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);
  UARTIntClear(UART0_BASE, UART_INT_RX);  
  UARTIntEnable(UART0_BASE, UART_INT_RX);
  UARTEnable(UART0_BASE); 
  IntEnable(INT_UART0);
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
  tau=0;
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
  waitN(ADC_FREQUENCY);
  BMin=averageADC(100);
  coil(1);
  waitN(ADC_FREQUENCY);
  BMax=averageADC(100);
  BMax=BOf(BMax);
  coil(0);
  waitN(ADC_FREQUENCY);

  uint32_t sumUp=0, sumDown=0;
  for(int i=0; i<10; ++i){
    sumUp+=tareUp();
    sumDown+=tareDown();
  }
  tauUp=((float)sumUp)/10;
  tauDown=((float)sumDown)/10;

  upRate=1-1.0f/(1+1.0f/tauUp+1.0f/tauUp/tauUp/2.0f);

  downRate=-1+1/(1+1.0f/tauDown+1.0f/tauDown/tauDown/2.0f);

  return 1;
}

int main(void)
{
  //Configure System Clock to Run with 16Mhz crystal in Main Oscillator. Use PLL (400MHz). Divide by 5 (There is also, a default Divide by 2)
  //Hence, Divide by 10. So, Clock Frequency = 400/10 = 40MHz.
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Enable Port F Peripheral
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);

  spettacolino();
  setupUART();
  setupADC();
  IntMasterEnable();

  heatUp();
  led(1);
  tareSensorValues();
  led(0);

  unsigned int i=0, j=0;
  coil(0);
  waitN(ADC_FREQUENCY);
  float currentValue=0, power=0;
  while(1)
  {
    wait();
    led(1);
    i++;
    if(i==ADC_FREQUENCY/10){
      i=0;
    }
    float coilB=currentValue*BMax;
    float filteredValue=B-coilB;
    if(squareWave){
      if(i==0){
        coil(0);
      }
      if(i==(ADC_FREQUENCY/20)){
        coil(1);
      }
      if(!(i%TICK_PER_SEND)){
        if((outputUp && coilOn) || (outputDown && !coilOn)){
          send(filteredValue);
        }
      }
    }
    else{
      float error=basePosition-filteredValue;
      if(power < currentValue){
        coil(0);
      }
      else{
        coil(1);
      }
      power=gainPos*error+gainBase;
      if(power < 0){
        power=0;
      }
      if(power > 1){
        power=1;
      }
      if(filteredValue < 100){
        power=0;
      }
      if(!(i%TICK_PER_SEND)){
        if((outputUp && coilOn) || (outputDown && !coilOn)){
          send (power *256);
        }
      }
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

    led(0);
  }
}
