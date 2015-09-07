#include <stdint.h>
#ifdef DEFINE_VARIABLES
#define EXTERN                  /* nothing */
#define INITIALIZER(...)        = __VA_ARGS__
#else
#define EXTERN                  extern
#define INITIALIZER(...)        /* nothing */
#endif /* DEFINE_VARIABLES */
EXTERN volatile char iMustDoTheControl INITIALIZER(0);
EXTERN volatile uint16_t lastADC;
EXTERN volatile float B;
EXTERN volatile float BMax, BMin;
EXTERN volatile char squareWave INITIALIZER(0);
EXTERN volatile float gainBase INITIALIZER(1), gainPos INITIALIZER(0.01);

EXTERN volatile char outputUp INITIALIZER(1), outputDown INITIALIZER(1);
EXTERN volatile float upRate, downRate;

EXTERN volatile char mustWriteData INITIALIZER(0);
