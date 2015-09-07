LIBRARY IEEE;
use IEEE.std_logic_1164.ALL;
ENTITY MouseHandlerTB IS
END MouseHandlerTB;

ARCHITECTURE TB OF MouseHandlerTB IS
signal THELEVEL:  std_logic_vector(6 downto 0);
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


CLK <= not CLK after 1.25ms;
test: PROCESS BEGIN
	BTN1<='0';
	BTN2<='0';
	BTN3<='0';
	RST <= '0';
	wait for 1ms;
	RST <= '1';
	wait for 1ms;
	RST <= '0';
	wait for 1ms;
	for i in 0 to 10 loop
		BTN1<='1';
		wait for 1ms;
		BTN1<='0';
		wait for 1ms;
	end loop;
	BTN3 <= '1';
	wait for 1ms;
	BTN3 <= '0';
	wait for 1ms;
	for i in 0 to 200 loop
		BTN1<='1';
		wait for 1ms;
		BTN1<='0';
		wait for 1ms;
	end loop;
		for i in 0 to 500 loop
		BTN2<='1';
		wait for 1ms;
		BTN2<='0';
		wait for 1ms;
	end loop;
	wait;
END PROCESS;
END ARCHITECTURE;
