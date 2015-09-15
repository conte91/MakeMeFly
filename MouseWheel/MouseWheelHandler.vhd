LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY MouseWheelHandler IS
PORT(
THELEVEL: OUT std_logic_vector(7 downto 0);
BTN1: IN std_logic;
BTN2: IN std_logic;
BTN3: IN std_logic;
RST: IN std_logic;
BTNCLK: IN std_logic;
OVERFLOW: OUT std_logic;
UNDERFLOW: OUT std_logic
);
END MouseWheelHandler;

ARCHITECTURE Structural OF MouseWheelHandler IS
signal theSum: unsigned(7 downto 0);
signal whatToSum: unsigned(7 downto 0);
signal myLevel: unsigned(7 downto 0);
signal newValue: unsigned(7 downto 0);
signal btnDown: std_logic;
signal BTN1_r, BTN2_r, BTN3_r, BTN1_p, BTN2_p: std_logic;
signal have_done_overflow,have_done_underflow: std_logic;
signal bypass: std_logic;
BEGIN
twoFlipFlops: PROCESS(BTNCLK) BEGIN
	IF(BTNCLK'event and BTNCLK='1') THEN
	BTN1_p<=BTN1_r;
	BTN2_p<=BTN2_r;
	BTN1_r<=BTN1;
	BTN2_r<=BTN2;
	BTN3_r<=BTN3;
	BTNDown<=BTN1 or BTN2;
	END IF;
END PROCESS;

whatToSumMux: PROCESS(BTN1_p) BEGIN
	IF(BTN1_p='1') THEN
		whatToSum <= (0 => '1', OTHERS => '0');
	ELSE
		whatToSum <= (OTHERS => '1');
	END IF;
END PROCESS;

theSum <= mylevel + whatToSum;

bypass <= ((BTN1_p and have_done_overflow) OR (BTN2_p and have_done_underflow));

bypassSumMux: PROCESS(bypass, mylevel, thesum) BEGIN
	IF(bypass='1') THEN
		newValue <= mylevel;
	ELSE
		newValue <= theSum;
	END IF;
END PROCESS;

doTheCalculation: PROCESS(BTNDown, RST, BTN3_r)
BEGIN 
  IF (RST='1' or BTN3_r='1') THEN
		mylevel<="01111111";
  ELSIF(BTNDown'event AND BTNDown='0') THEN
		mylevel<=newValue;
	END IF;
END PROCESS;

THELEVEL <= std_logic_vector(myLevel);
have_done_overflow<=  (mylevel(7) and mylevel(6) and mylevel(5) and mylevel(4) and mylevel(3) and mylevel(2) and mylevel(1) and mylevel(0));
have_done_underflow <= (not mylevel(7)) and (not mylevel(6)) and (not mylevel(5));
OVERFLOW <= have_done_overflow;
UNDERFLOW <= have_done_underflow;
END ARCHITECTURE;
