LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

ENTITY Reg
IS
  GENERIC(
  RegSize: integer
);

PORT (
       input: IN std_logic_vector((RegSize-1) DOWNTO 0);
       output: OUT std_logic_vector((RegSize-1) DOWNTO 0):= (OTHERS => '0');
       serialOut: OUT std_logic:='0';
       serialIn: IN std_logic;
       clk: IN std_logic;
       writeEn: IN std_logic;
       serialWriteEn: IN std_logic
     );
END Reg;

ARCHITECTURE Reg_Beh OF Reg IS
BEGIN
  regprocess: PROCESS(clk, writeEn, input)
  VARIABLE val: std_logic_vector((RegSize-1) DOWNTO 0) := (OTHERS => '0');
  BEGIN
    IF(clk'event AND clk='1') THEN
      IF(serialWriteEn='1') THEN
        serialOut<=val(0);
        val:= serialIn & val((RegSize-1) DOWNTO 1);
        output<=val;
      END IF;
    END IF;
    --Async parallel write
    IF(writeEn='1') THEN
      val := input;
      output<=val;
    END IF;
  END PROCESS;
END Reg_Beh;
