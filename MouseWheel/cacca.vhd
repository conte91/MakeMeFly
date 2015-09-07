LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY MouseWheelHandler IS
PORT(
THELEVEL: OUT std_logic_vector(6 downto 0);
BTN1: IN std_logic;
BTN2: IN std_logic;
BTN3: IN std_logic;
RST: IN std_logic;
WHEEL1: IN std_logic;
WHEEL2: IN std_logic;
OVERFLOW: OUT std_logic;
UNDERFLOW: OUT std_logic

);
END MouseWheelHandler;

ARCHITECTURE Structural OF MouseWheelHandler IS
signal toBeAdded: unsigned(6 downto 0);
signal newValue: unsigned(6 downto 0);
signal level: unsigned(6 downto 0);
signal level_bits: std_logic_vector(6 downto 0);
signal theSum: unsigned(6 downto 0);
signal theSum_bits: std_logic_vector(6 downto 0);
signal have_done_overflow: std_logic;
signal have_done_underflow: std_logic;
signal bypass: std_logic;
signal btnDown: std_logic;
signal summingUp: std_logic;
signal theRST: std_logic;
signal rst_n: std_logic;
signal btnDown_n: std_logic;


BEGIN
btnDown <= BTN1 or BTN2;
have_done_overflow<=  (level(6) and level(5) and level(4) and level(3) and level(2) and level(1) and level(0));
have_done_underflow <= (not level(6)) and (not level(5)) and (not level(4));
bypass <= (have_done_overflow and summingUp) or (have_done_underflow and (not summingUp));
theRST <= RST or BTN3;

toBeAdded<="0000001";
whatToSum: PROCESS(BTNDown) BEGIN
	IF(BTNDown'event AND BTNDown='1') THEN
		IF(BTN1='1') THEN
			newValue<=level + toBeAdded;
			summingUp<= '1';
		ELSE
			newValue<= level-toBeAdded;
			summingUp<= '0';
		END IF;
	END IF;
END PROCESS;

rst_n <= not theRST;
btnDown_n <= not btnDown;
currentValue: ENTITY work.Reg GENERIC MAP(7, 63)
PORT MAP(input => theSum_bits,
       output=> level_bits,
    		 rst => rst_n,
       writeCLK => btnDown_n
		 );
level <= unsigned(level_bits);
theSum <= level when bypass='1' else newValue;
theSum_bits <= std_logic_vector(theSum);
OVERFLOW <= have_done_overflow;
UNDERFLOW <= have_done_underflow;
THELEVEL <= level_bits;		 
END ARCHITECTURE;
