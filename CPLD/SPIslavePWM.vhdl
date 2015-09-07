LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY spiSlavePWM IS
  GENERIC(wordSize: integer);

  PORT(
    MOSI: IN std_logic;
    CS: IN std_logic;
    CLK: IN std_logic;
    pwmCLK: IN std_logic;
    output: OUT std_logic
  );
END spiSlavePWM;

ARCHITECTURE spiSlaveArch OF spiSlavePWM IS
  SIGNAL pwmValue: unsigned(wordSize-1 DOWNTO 0) := (OTHERS => '0');
  SIGNAL regOutput: std_logic_vector(wordSize-1 DOWNTO 0);
BEGIN
  --We negate the clock, so sampling occours on the falling edge
  shiftReg: ENTITY work.Reg GENERIC MAP (wordSize) PORT MAP ((OTHERS => '0'), regOutput, OPEN, MOSI, "not"(CLK), '0', "not"(CS));

  valuep: PROCESS(CS)
  BEGIN
    -- Did we get a new value from the master?
    IF CS'event AND CS='1' THEN
      pwmValue<=unsigned(regOutput);
    END IF;
  END PROCESS;
  
  --Count up to 2^N, goes to 0 when counter reaches pwmValue
  pwmProc: PROCESS(pwmCLK)
    VARIABLE counter: unsigned(wordSize-1 DOWNTO 0) := (OTHERS => '0');
    CONSTANT ceiling: INTEGER :=2**wordSize;
  BEGIN
    IF pwmCLK='1' THEN
      -- We use = and not >, in this way pwm gets always either old value or new value if SPI value is changed between PWM periods, and does not reach intermediate values.
      IF counter=pwmValue THEN
        output<='0';
      END IF;
      counter:=counter+1;
      IF(counter=0) THEN
        output<='1';
      END IF;
    END IF;
  END PROCESS;
END spiSlaveArch;




    
