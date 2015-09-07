PROJECT(Levitor)

SET(CMAKE_C_COMPILER msp430-gcc)
SET(CMAKE_C_FLAGS "-mmcu=msp430g2553 -Wall -g -O2 -std=c99")

INCLUDE_DIRECTORIES(".")
add_subdirectory(MSP_Dco/)
add_subdirectory(ADC/)
add_subdirectory(CBuf/)
add_subdirectory(UART/)
add_subdirectory(ConstantField/)
SET(BIN_NAME adc_test)
ADD_EXECUTABLE(${BIN_NAME} adc_test_1.c ADC/adc.c CBuf/cbuf.c MSP_Dco/dco_custom_calibration.c UART/uart.c)
