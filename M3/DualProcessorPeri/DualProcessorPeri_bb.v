
module DualProcessorPeri (
	accel_I2C_SDAT,
	accel_I2C_SCLK,
	accel_G_SENSOR_CS_N,
	accel_G_SENSOR_INT,
	buzzer_export,
	hex012_export,
	hex345_export,
	image_address_export,
	keys_export,
	led_green_export,
	led_red_export,
	led_yellow_export,
	ledr_export,
	pixel_buffer_export,
	shared_sdram_wire_addr,
	shared_sdram_wire_ba,
	shared_sdram_wire_cas_n,
	shared_sdram_wire_cke,
	shared_sdram_wire_cs_n,
	shared_sdram_wire_dq,
	shared_sdram_wire_dqm,
	shared_sdram_wire_ras_n,
	shared_sdram_wire_we_n,
	spi_MISO,
	spi_MOSI,
	spi_SCLK,
	spi_SS_n,
	spi_ss_export,
	sw_export,
	sys_sdram_pll_0_ref_clk_clk,
	sys_sdram_pll_0_ref_reset_reset,
	sys_sdram_pll_0_sdram_clk_clk,
	us_counter_export,
	wern_export);	

	inout		accel_I2C_SDAT;
	output		accel_I2C_SCLK;
	output		accel_G_SENSOR_CS_N;
	input		accel_G_SENSOR_INT;
	output		buzzer_export;
	output	[23:0]	hex012_export;
	output	[23:0]	hex345_export;
	output	[16:0]	image_address_export;
	input	[1:0]	keys_export;
	output		led_green_export;
	output		led_red_export;
	output		led_yellow_export;
	output	[7:0]	ledr_export;
	output	[3:0]	pixel_buffer_export;
	output	[12:0]	shared_sdram_wire_addr;
	output	[1:0]	shared_sdram_wire_ba;
	output		shared_sdram_wire_cas_n;
	output		shared_sdram_wire_cke;
	output		shared_sdram_wire_cs_n;
	inout	[15:0]	shared_sdram_wire_dq;
	output	[1:0]	shared_sdram_wire_dqm;
	output		shared_sdram_wire_ras_n;
	output		shared_sdram_wire_we_n;
	input		spi_MISO;
	output		spi_MOSI;
	output		spi_SCLK;
	output		spi_SS_n;
	output		spi_ss_export;
	input	[9:0]	sw_export;
	input		sys_sdram_pll_0_ref_clk_clk;
	input		sys_sdram_pll_0_ref_reset_reset;
	output		sys_sdram_pll_0_sdram_clk_clk;
	input	[31:0]	us_counter_export;
	output		wern_export;
endmodule
