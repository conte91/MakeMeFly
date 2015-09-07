LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

ENTITY mux21 IS
PORT(
S: in std_logic;
A: in std_logic;
B: in std_logic;
O: out std_logic
);
END mux21;

ARCHITECTURE beh OF mux21 IS
BEGIN
	--O <= (S and B) or ((not S) and A);
	O <= B when S = '1' else A;
END ARCHITECTURE;