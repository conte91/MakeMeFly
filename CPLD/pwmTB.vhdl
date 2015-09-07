LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
ENTITY pwmTB IS
  END pwmTB;

ARCHITECTURE pwmTBArch OF pwmTB IS
  CONSTANT wordSize: INTEGER :=8;
  CONSTANT pwmperiod: INTEGER :=2**wordSize;
  SIGNAL output: std_logic:='0';
  SIGNAL spiclk: std_logic := '0';
  SIGNAL pwmclk: std_logic := '0';
  SIGNAL upclk: std_logic := '0';
  SIGNAL data: std_logic := '0';
  SIGNAL cs: std_logic;
  SIGNAL ivalue: signed(wordSize-1 DOWNTO 0) := (OTHERS => '0');
  SIGNAL regWEn: std_logic := '0';
  SIGNAL finished: std_logic := '0';

BEGIN

  spiReg: ENTITY work.Reg GENERIC MAP(wordSize) PORT MAP(std_logic_vector(ivalue), OPEN, data, '0', spiclk, regWEn, "not"(cs));

  --Clock per il PWM
  pwmclkp: PROCESS
  BEGIN
	 --12MHz clock
    IF(finished='0') THEN
      pwmclk <= '0';
      WAIT FOR 41 ns;
      pwmclk <= '1';
      WAIT FOR 41 ns;
    ELSE
      WAIT;
    END IF;
  END PROCESS;

  upclkp: PROCESS
  BEGIN
    IF(finished='0') THEN
      upclk <='0';
      WAIT FOR 5 us;
      upclk <= '1';
      WAIT FOR 5 us;
    ELSE
      WAIT;
    END IF;
  END PROCESS;


  updateValue: PROCESS
  BEGIN
    cs<='1';
    FOR i in 0 TO pwmPeriod LOOP
      WAIT UNTIL upclk='1';
      --Puts value in send register
      ivalue <= ivalue + 1;
      regWEn<='1';
      WAIT FOR 2 ns;
      regWEn <= '0';
      --Sends new value
      cs<='0';
      WAIT FOR 10 ns;
      --Sends Clock
      FOR i IN 1 TO wordSize LOOP
        spiclk<='1';
        WAIT FOR 100 ns;
        spiclk<='0';
        WAIT FOR 100 ns;
      END LOOP;
      cs <= '1';
    END LOOP;
    finished<='1';
  END PROCESS;

  --
  pwm: ENTITY work.spiSlavePWM GENERIC MAP (8) PORT MAP(data, cs, spiclk, pwmclk, output);
END pwmTBArch;
