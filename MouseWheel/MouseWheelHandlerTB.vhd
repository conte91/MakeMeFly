LIBRARY IEEE;
use IEEE.std_logic_1164.ALL;
ENTITY MouseHandlerTB IS
END MouseHandlerTB;

ARCHITECTURE TB OF MouseHandlerTB IS
signal THELEVEL:  std_logic_vector(7 downto 0);
signal BTN1:  std_logic;
signal BTN2:  std_logic;
signal BTN3:  std_logic;
signal RST:  std_logic;
signal CLK: std_logic:='0';
signal OVERFLOW:  std_logic;
signal UNDERFLOW:  std_logic;
BEGIN

uut: ENTITY work.MouseWheelHandler 
PORT MAP(THELEVEL, BTN1, BTN2, BTN3, RST, CLK, OVERFLOW, UNDERFLOW);

clockit: PROCESS BEGIN
  FOR i in 0 to 20000 loop
    wait for 0.22ms;
    CLK <= not CLK;
  END LOOP;
  wait;
END PROCESS;

test: PROCESS BEGIN
	BTN1<='0';
	BTN2<='0';
	BTN3<='0';
	RST <= '0';
	wait for 2ms;
	RST <= '1';
	wait for 2ms;
	RST <= '0';
	wait for 2ms;
	for i in 0 to 10 loop
		BTN1<='1';
		wait for 2ms;
		BTN1<='0';
		wait for 2ms;
	end loop;
	BTN3 <= '1';
	wait for 2ms;
	BTN3 <= '0';
	wait for 2ms;
	for i in 0 to 200 loop
		BTN1<='1';
		wait for 2ms;
		BTN1<='0';
		wait for 2ms;
	end loop;
		for i in 0 to 500 loop
		BTN2<='1';
		wait for 2ms;
		BTN2<='0';
		wait for 2ms;
	end loop;
	wait;
END PROCESS;
END ARCHITECTURE;
