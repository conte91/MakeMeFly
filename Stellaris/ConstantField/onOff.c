#include <inc/hw_memmap.h> 
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <inc/hw_ssi.h>
#include <driverlib/debug.h>
#include <driverlib/sysctl.h> 
#include <driverlib/adc.h> 
#include <driverlib/interrupt.h>
#include <driverlib/uart.h>
#include <driverlib/gpio.h> 
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include <driverlib/ssi.h>

#include "variables.h"
#ifdef DEBUG //Error Checking on APIs
void__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif
#define ADC_FREQUENCY 10000
#define SEND_FREQUENCY 100
#define TICK_PER_CONTROL 1
#define TICK_PER_SEND 1
#define SIZE_OF_CALIBRATION 6
#define CALIBRATION_ADDRESS 0x00000000

float coilB;
float filteredValue;
char coilOn;
uint16_t basePosition INITIALIZER(0);

void readCalibrationFromFlash();

static inline void send(const uint16_t value){
  UARTCharPutNonBlocking(UART0_BASE, value & 0xFF);
  UARTCharPutNonBlocking(UART0_BASE, (value >> 8) & 0xFF);
}

void errorSend(const char* message){
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  const char* mazzi="Mazzi\n";
  if(message==0){
    while(1);
    const char* ptr=mazzi;
    while((*ptr)!='\0'){
      UARTCharPut(UART0_BASE, *ptr);
      ptr++;
    }
  }
  while(1){
    const char* ptr=message;
    while((*ptr)!='\0'){
      UARTCharPut(UART0_BASE, *ptr);
      ptr++;
    }
  }
}
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
static inline float BOf(float reading){
  return -(reading-BMin);
}

static inline void cs(const char value){
  if(value){
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);
  }
  else{
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
  }
  /** Waits 1us */
  SysCtlDelay(SysCtlClockGet()/1000000);
}

void heatUp(){
  /** Heat up? */
  for(int i=0; i<5; ++i){
    coil(1);
    for(int i=0; i<ADC_FREQUENCY; ++i){
      wait();
    }
    coil(0);
    for(int i=0; i<ADC_FREQUENCY; ++i){
      wait();
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

void setupTimer(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
  TimerEnable(TIMER1_BASE, TIMER_A);
  unsigned long timerPeriod=SysCtlClockGet() / SEND_FREQUENCY;
  TimerLoadSet(TIMER1_BASE, TIMER_A, timerPeriod-1);
  TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  IntEnable(INT_TIMER1A);
}


static inline void SPISend(const char data){
  SSIDataPut(SSI0_BASE, data);
  unsigned long foo;
  SSIDataGet(SSI0_BASE, &foo);
}
static inline unsigned char readSPI(){
  unsigned long data;
  SSIDataPut(SSI0_BASE, 0xFF);
  SSIDataGet(SSI0_BASE, &data);
  return (data & 0xFF);
}
static inline void waitSSIPut(){
    while(SSIBusy(SSI0_BASE));
}

void readDataFromFlash(char* data, uint32_t address, unsigned int nBytes){

  cs(0);
  SPISend( 0x03);
  SPISend( (address >> 16) & 0xFF);
  SPISend( (address >> 8) & 0xFF);
  SPISend( address & 0xFF);

  for(unsigned int i=0; i<nBytes; ++i){
    (*data)=readSPI();
    data++;
  }

  cs(1);
}

uint32_t readFlashID(){
  cs(0);
  SPISend( 0x9F);
  waitSSIPut();
  uint32_t result=((uint32_t)readSPI() << 16 );
  result |=((uint32_t)readSPI() << 8) ;
  result |=readSPI();
  cs(1);
  return result;
}

static inline char readFlashStatusRegister(){
  cs(0);
  SPISend( 0x05);
  waitSSIPut();
  char result=readSPI();
  cs(1);
  return result;
}
void enableWriteOnFlash(){
  cs(0);
  SPISend( 0x06);
  waitSSIPut();
  cs(1);
  if(readFlashStatusRegister()!=0x02){
    errorSend("Couldn't enable flash\n");
  }
  
}

void writeDataToFlash(const char* data, uint32_t address, unsigned int nBytes){
    
  enableWriteOnFlash();

  cs(0);
  SPISend(0x20);
  SPISend( (address >> 16) & 0xFF);
  SPISend( (address >> 8) & 0xFF);
  SPISend( address & 0xFF);
  waitSSIPut();
  cs(1);
  while(readFlashStatusRegister() & 0x01);
  
  enableWriteOnFlash();
  cs(0);
  SPISend( 0x02);
  SPISend( (address >> 16) & 0xFF);
  SPISend( (address >> 8) & 0xFF);
  SPISend( address & 0xFF);
  waitSSIPut();

  const char* ptr=data;
  for(unsigned int i=0; i<nBytes; ++i){
    SPISend(*ptr);
    waitSSIPut();
    ptr++;
  }

  cs(1);
  while(readFlashStatusRegister() & 0x01);
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
  UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX7_8,UART_FIFO_RX1_8);
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
  float tauUp, tauDown;
  tauUp=((float)sumUp)/10;
  tauDown=((float)sumDown)/10;

  upRate=1-1.0f/(1+1.0f/tauUp+1.0f/tauUp/tauUp/2.0f);

  downRate=-1+1/(1+1.0f/tauDown+1.0f/tauDown/tauDown/2.0f);

  return 1;
}
void setupSPI(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
  /* Manual CS on PF.4 */
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7);
  GPIOPinConfigure(GPIO_PA2_SSI0CLK);
  //GPIOPinConfigure(GPIO_PA3_SSI0FSS);
  GPIOPinConfigure(GPIO_PA4_SSI0RX);
  GPIOPinConfigure(GPIO_PA5_SSI0TX);
  GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);
  SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);
  SSIEnable(SSI0_BASE);
  char error[23]="Wrong flash ID! xxxxxx\n";
  uint32_t n=readFlashID();
  if(n!=0x00373014){
    error[16]=(n>>20 & 0x0F)+'0';
    error[17]=(n>>16 & 0x0F)+'0';
    error[18]=(n>>12 & 0x0F)+'0';
    error[19]=(n>>8 & 0x0F)+'0';
    error[20]=(n>>4 & 0x0F)+'0';
    error[21]=(n & 0x0F)+'0';

    errorSend(error);
  }
}

void setupGPIO(){
  /* For reading evvverythin */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); 
  GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, 0xFF); 

  /* For LEDs */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Enable Port F Peripheral
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4); 
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
}

void writeCalibrationToFlash(){
  float data[SIZE_OF_CALIBRATION];
  data[0]=BMax;
  data[1]=BMin;
  data[2]=gainBase;
  data[3]=gainPos;
  data[4]=upRate;
  data[5]=downRate;
  writeDataToFlash((char*) data, CALIBRATION_ADDRESS, (SIZE_OF_CALIBRATION*sizeof(float))/sizeof(char));
  readCalibrationFromFlash();

  if(data[0]!=BMax ||data[1]!=BMin || data[2]!=gainBase || data[3]!=gainPos || data[4]!=upRate || data[5]!=downRate){
    errorSend("For the beard of Dumbledore!\n");
  }
}

void readCalibrationFromFlash(){
  float data[SIZE_OF_CALIBRATION];
  readDataFromFlash((char*) data, CALIBRATION_ADDRESS, (SIZE_OF_CALIBRATION*sizeof(float))/sizeof(char));
  /** All of this is to avoid reading NaN from uninitialized memory, otherwise writing it again to memory would cause an error as NaN!=NaN */
  BMax=(data[0]==data[0] ? data[0] : 0.0f);
  BMin=(data[1]==data[1] ? data[1] : 0.0f);
  gainBase=(data[2]==data[2] ? data[2] : 0.0f);
  gainPos=(data[3]==data[3] ? data[3] : 0.0f);
  upRate=(data[4]==data[4] ? data[4] : 0.0f);
  downRate=(data[5]==data[5] ? data[5] : 0.0f);
}

int main(void)
{
  //Configure System Clock to Run with 16Mhz crystal in Main Oscillator. Use PLL (400MHz). Divide by 5 (There is also, a default Divide by 2)
  //Hence, Divide by 10. So, Clock Frequency = 400/10 = 40MHz.
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);


  setupGPIO();
  setupUART();
  setupSPI();
  setupADC();
  spettacolino();
  IntMasterEnable();

  //heatUp();
  led(1);
  readCalibrationFromFlash();
  led(0);

  unsigned int i=0;
  coil(0);
  waitN(ADC_FREQUENCY);
  float currentValue=0, power=0;
  //setupTimer();
  char whatImSendingNow=0;

  while(1)
  {
    /** Sync */
    wait();
    led(1);
    if(mustWriteData){
      writeCalibrationToFlash();
      mustWriteData=0;
      UARTCharPut(UART0_BASE, 'A');
      UARTCharPut(UART0_BASE, 'C');
      UARTCharPut(UART0_BASE, 'K');
    }
    if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){
      tareSensorValues();
    }

    char sw,oup,od,co;
    
    switch(whatImSendingNow){

      case 0:
        UARTCharPutNonBlocking(UART0_BASE, 'L');
        UARTCharPutNonBlocking(UART0_BASE, 'V');
        UARTCharPutNonBlocking(UART0_BASE, 'T');
        break;
      case 1:
        UARTCharPutNonBlocking(UART0_BASE, (unsigned char) (basePosition>>4) );
        send( (int16_t) (gainPos*10000.0f));
        send( (int16_t) ((gainBase-1)*1000.0f) );
        break;
      case 2:
        send( (int16_t) (upRate*10000) );
        send( (int16_t) (downRate*10000) );
        break;
      case 3:
        sw=squareWave;
        oup=outputUp;
        od=outputDown;
        co=coilOn;

        UARTCharPutNonBlocking(UART0_BASE, 0x00 | (sw ? 0x01 : 0x00) | (oup ? 0x02 : 0x00) | (od ? 0x04 : 0x00) | (co ? 0x08 : 0x00) );
        if((oup && co) || (od && !co)){
          /** Sends state */
          send(filteredValue*256);
          send(coilB*256);
          send(B*256);
        }
        break;
     }

    whatImSendingNow++;
    if(whatImSendingNow==4){
      whatImSendingNow=0;
    }
    i++;
    if(i==ADC_FREQUENCY/10){
      i=0;
    }
    coilB=currentValue*BMax;
    filteredValue=B-coilB;
    if(squareWave){
      if(i==0){
        coil(0);
      }
      if(i==(ADC_FREQUENCY/20)){
        coil(1);
      }
    }
    else{
      basePosition=GPIOPinRead(GPIO_PORTB_BASE, 0xFF) << 4;
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
      if(filteredValue < 500){
        power=0;
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
