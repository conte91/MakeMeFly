PROJECT(Levitor)

SET(CMAKE_C_COMPILER msp430-gcc)
SET(CMAKE_C_FLAGS "-mmcu=msp430g2553 -Wall -g -O0")

SET(BIN_NAME adc_test)
ADD_EXECUTABLE(${BIN_NAME} adc_test_1.c ADC/adc.c CBuf/cbuf.c MSP_Dco/dco_custom_calibration.c UART/uart.c)
INCLUDE_DIRECTORIES(.)

ADD_CUSTOM_TARGET(flash
  mspdebug rf2500 'prog ${BIN_NAME}'
  DEPENDS ${BIN_NAME}
  )

