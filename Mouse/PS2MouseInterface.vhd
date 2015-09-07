LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

ENTITY PS2MouseInterface
IS
PORT (
       dataIn: IN std_logic;
       devCLK: IN std_logic;
		 dataOut: OUT std_logic;
		 inhibitCLK: OUT std_logic;
		 mouseMovedUp: OUT std_logic;
		 mouseMovedDown: OUT std_logic;
		 brokenPacket: OUT std_logic;
		 --rcv_packet: OUT std_logic_vector(32 downto 0);

		 goodOp: OUT std_logic
     );
	  
END PS2MouseInterface;

ARCHITECTURE Structural OF PS2MouseInterface IS
	SIGNAL broken_packet: std_logic;
	SIGNAL up: std_logic;
	SIGNAL down: std_logic;
	SIGNAL loadPkt: std_logic;
	SIGNAL pktOut: std_logic;
	SIGNAL devIsMaster: std_logic;
	--SIGNAL rcv_packet: std_logic_vector(32 downto 0);
	SIGNAL snd_packet: std_logic_vector(32 downto 0);

BEGIN

	brokenPacket <= broken_packet;
	goodOp <= not broken_packet;
	bypass_up_if_broken_packet: ENTITY work.mux21
	PORT MAP(S => broken_packet, A => up, B => '0', O => mouseMovedUp);
	bypass_down_if_broken_packet: ENTITY work.mux21
	PORT MAP(S => broken_packet, A => down, B => '0', O => mouseMovedDown);
	packet_reg: ENTITY work.Reg GENERIC MAP(33) PORT MAP(
	    input => snd_packet,
       output => rcv_packet,
       serialOut=> pktOut,
       serialIn=>  dataIn,
       clk=> devCLK,
       writeEn=> loadPkt,
       serialWriteEn=> devIsMaster
	);
END ARCHITECTURE;


