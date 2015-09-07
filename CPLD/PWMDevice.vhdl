LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
ENTITY PWMDevice IS
PORT(
	MOSI: IN std_logic;
	CS: IN std_logic;
	CLK: IN std_logic;
    pwmCLK: IN std_logic;
    output: OUT std_logic
  );
END PWMDevice;

ARCHITECTURE PwmDeviceArch of PWMDevice IS
BEGIN
	dev: ENTITY work.SPIslavePWM GENERIC MAP (8) PORT MAP (MOSI, CS, CLK, pwmCLK, output);
END PwmDeviceArch;