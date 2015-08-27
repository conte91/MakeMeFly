ADD_EXECUTABLE(onOff onOff.c uart.c ../MSP_Dco/dco_custom_calibration.c ../CBuf/cbuf.c)
TARGET_LINK_LIBRARIES(onOff m)
ADD_EXECUTABLE(testPWM testPWM.c ../MSP_Dco/dco_custom_calibration.c ../ADC/adc.c ../CBuf/cbuf.c ../UART/uart.c)
ADD_EXECUTABLE(constantField constantField.c ../MSP_Dco/dco_custom_calibration.c ../ADC/adc.c ../CBuf/cbuf.c ../UART/uart.c)
TARGET_LINK_LIBRARIES(constantField m)

