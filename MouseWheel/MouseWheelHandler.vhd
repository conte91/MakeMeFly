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
--signal toBeAdded: unsigned(6 downto 0);
--signal newValue: unsigned(6 downto 0);
--signal level_bits: std_logic_vector(6 downto 0);
--signal theSum: unsigned(6 downto 0);
--signal theSum_bits: std_logic_vector(6 downto 0);
signal myLevel: unsigned(7 downto 0);
signal btnDown: std_logic;
signal BTN1_r, BTN2_r, BTN3_r, BTN1_p, BTN2_p: std_logic;
signal have_done_overflow,have_done_underflow: std_logic;
BEGIN
--NBTN1 <= not BTN1_r;
--NBTN2 <= not BTN2_r;
--NBTN3 <= not BTN3_r;
--NBTNCLK <= btnDown_n;
--NRST <= not theRST;

--BTN1_p<=BTN1;
--BTN2_p<=BTN2;
--BTN1_r<=BTN1;
--BTN2_r<=BTN2;
--BTN3_r<=BTN3;
--BTNDown<=BTN1 or BTN2;
regRobe: PROCESS(BTNCLK) BEGIN
	IF(BTNCLK'event and BTNCLK='1') THEN
	BTN1_p<=BTN1_r;
	BTN2_p<=BTN2_r;
	BTN1_r<=BTN1;
	BTN2_r<=BTN2;
	BTN3_r<=BTN3;
	BTNDown<=BTN1 or BTN2;
	END IF;
END PROCESS;

doTheCalculation: PROCESS(BTNDown, RST, BTN3_r)
  VARIABLE level: unsigned(7 downto 0);
  VARIABLE maybe: unsigned(7 downto 0);
BEGIN 
  IF (RST='1' or BTN3_r='1') THEN
		level:="01111111";
		mylevel<=level;
  ELSIF(BTNDown'event AND BTNDown='0') THEN
		IF (BTN1_p='1' AND ((level(7) and level(6) and level(5) and level(4) and level(3) and level(2) and level(1) and level(0)))='0') THEN
			level := level+"00000001";
		mylevel<=level;
		ELSIF(BTN2_p='1' AND (((not level(7)) and (not level(6)) and (not level(5))))='0') THEN
			level := level-"00000001";
		mylevel<=level;
		END IF;
	END IF;
END PROCESS;
		
	
--regRobe: PROCESS(BTNCLK) BEGIN
--	IF(BTNCLK'event and BTNCLK='1') THEN
--		BTN1_r<=BTN1;
--		BTN2_r<=BTN2;
--		BTN3_r<=BTN3;
--	END IF;
--END PROCESS;
--	
THELEVEL <= std_logic_vector(myLevel);
--btnDown <= BTN1_r or BTN2_r;
have_done_overflow<=  (mylevel(7) and mylevel(6) and mylevel(5) and mylevel(4) and mylevel(3) and mylevel(2) and mylevel(1) and mylevel(0));
have_done_underflow <= (not mylevel(7)) and (not mylevel(6)) and (not mylevel(5));
--bypass <= (have_done_overflow and summingUp) or (have_done_underflow and (not summingUp));
--theRST <= RST or BTN3_r;
--
--toBeAdded<="0000001";
--whatToSum: PROCESS(BTNDown) BEGIN
--	IF(BTNDown'event AND BTNDown='1') THEN
--		IF(BTN1_r='1') THEN
--			newValue<=level + toBeAdded;
--			summingUp<= '1';
--		ELSE
--			newValue<= level-toBeAdded;
--			summingUp<= '0';
--		END IF;
--	END IF;
--END PROCESS;
--
--rst_n <= not theRST;
--btnDown_n <= not btnDown;
----currentValue: PROCESS(theRST, BTNCLK)
--currentValue: ENTITY work.Reg GENERIC MAP(7, 63)
--PORT MAP(input => theSum_bits,
--     output=> level_bits,
--    		 rst => rst_n,
--       writeCLK => btnDown_n
--		 );
--level <= unsigned(level_bits);
--theSum <= level when bypass='1' else newValue;
--theSum_bits <= std_logic_vector(theSum);
OVERFLOW <= have_done_overflow;
UNDERFLOW <= have_done_underflow;
--THELEVEL <= level_bits;		 
--BYPASS_P <= not bypass;
--SUM <= theSum_bits;
END ARCHITECTURE;
