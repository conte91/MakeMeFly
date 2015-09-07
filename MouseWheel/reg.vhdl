LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
ENTITY Reg
IS
  GENERIC(
  RegSize: integer;
  startValue: integer

);

PORT (
       input: IN std_logic_vector((RegSize-1) DOWNTO 0);
       output: OUT std_logic_vector((RegSize-1) DOWNTO 0);
		 rst: IN std_logic;
       writeCLK: IN std_logic
     );
END Reg;

ARCHITECTURE Reg_Beh OF Reg IS
BEGIN
  regprocess: PROCESS(writeCLK, rst)
  VARIABLE val: std_logic_vector((RegSize-1) DOWNTO 0) := (OTHERS => '0');
  BEGIN
    --Async reset
	 IF(rst='0') THEN
		val := std_logic_vector(to_unsigned(startValue, RegSize));
		output <= val;
	 ELSIF(writeCLK'event and writeCLK='1') THEN
      val := input;
      output<=val;
    END IF;
  END PROCESS;
END Reg_Beh;
