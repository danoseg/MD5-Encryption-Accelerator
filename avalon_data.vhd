-- md5_data.vhd
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity md5_data is
	port (
		avs_s0_address  : in  std_logic_vector(3 downto 0)  := (others => '0'); --          s0.address
		avs_s0_read     : in  std_logic                     := '0';             --            .read
		avs_s0_write    : in  std_logic                     := '0';             --            .write
		avs_s0_readdata : out std_logic_vector(31 downto 0);                    --            .readdata
		avs_s0_writedata  : in  std_logic_vector(31 downto 0)  := (others => '0'); --          s0.address
		clk             : in  std_logic                     := '0';             --       clock.clk
		reset           : in  std_logic                     := '0';             --       reset.reset                   -- md5_input.m_in1
		md5_writeaddr   : out std_logic_vector(31 downto 0);                    -- md5_input.m_in2
		md5_writedata   : out std_logic_vector(31 downto 0);
		md5_readaddr	 : out std_logic_vector(31 downto 0);
		md5_readdata    : in std_logic_vector(31 downto 0)
	);
end entity md5_data;

architecture rtl of md5_data is

	SIGNAL writedata, writeaddr, readaddr : STD_LOGIC_VECTOR (31 DOWNTO 0);
	
begin
	PROCESS(clk, reset, avs_s0_read, avs_s0_write, avs_s0_address, avs_s0_writedata)
	BEGIN
		IF(reset = '1')THEN
			avs_s0_readdata <= (OTHERS => '0');
			writedata <= (OTHERS => '0');
			writeaddr <= (OTHERS => '0');
			readaddr <= (OTHERS => '0');
			
		ELSIF(rising_edge(clk))THEN
			IF(avs_s0_read = '1')THEN
				CASE avs_s0_address IS
					WHEN "0000" =>
						avs_s0_readdata <= writeaddr;
					WHEN "0001" =>
						avs_s0_readdata <= writedata;
					WHEN "0010" =>
						avs_s0_readdata <= md5_readdata;					
					WHEN OTHERS =>
						avs_s0_readdata <= (OTHERS => '0');
				END CASE;
				
			ELSIF(avs_s0_write = '1')THEN
				CASE avs_s0_address IS
					WHEN "0000" =>
						writeaddr <= avs_s0_writedata;
					WHEN "0001" =>
						writedata <= avs_s0_writedata;
					WHEN "0010" =>
						readaddr <= avs_s0_writedata;
					WHEN "0011" =>
						readaddr <= avs_s0_writedata(4 DOWNTO 0) & "000000000000000000000000000";
					WHEN "0100" =>
						writeaddr <= avs_s0_writedata(3 DOWNTO 0) & "0000000000000000000000000000";
					WHEN OTHERS =>
					
					END CASE;
			
			END iF;
		END IF;
	END PROCESS;
	
	md5_readaddr <= readaddr;
	md5_writedata <= writedata;
	md5_writeaddr <= writeaddr;
	
	
end architecture rtl; -- of md5_output

