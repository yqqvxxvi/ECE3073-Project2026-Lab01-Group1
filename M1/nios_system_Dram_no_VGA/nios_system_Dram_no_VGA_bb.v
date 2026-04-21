
module nios_system_Dram_no_VGA (
	accel_I2C_SDAT,
	accel_I2C_SCLK,
	accel_G_SENSOR_CS_N,
	accel_G_SENSOR_INT,
	buzzer_export,
	clk50_clk,
	dram_addr,
	dram_ba,
	dram_cas_n,
	dram_cke,
	dram_cs_n,
	dram_dq,
	dram_dqm,
	dram_ras_n,
	dram_we_n,
	green_light_export,
	hex012_export,
	hex345_export,
	key0_export,
	key1_export,
	latency_out_export,
	led8_export,
	msb_export,
	red_led_0_7_export,
	red_light_export,
	spi_MISO,
	spi_MOSI,
	spi_SCLK,
	spi_SS_n,
	spi_ss_export,
	sw0_8_export,
	uart_0_rxd,
	uart_0_txd,
	yellow_light_export);	

	inout		accel_I2C_SDAT;
	output		accel_I2C_SCLK;
	output		accel_G_SENSOR_CS_N;
	input		accel_G_SENSOR_INT;
	output		buzzer_export;
	input		clk50_clk;
	output	[12:0]	dram_addr;
	output	[1:0]	dram_ba;
	output		dram_cas_n;
	output		dram_cke;
	output		dram_cs_n;
	inout	[15:0]	dram_dq;
	output	[1:0]	dram_dqm;
	output		dram_ras_n;
	output		dram_we_n;
	output		green_light_export;
	output	[23:0]	hex012_export;
	output	[23:0]	hex345_export;
	input		key0_export;
	input		key1_export;
	output		latency_out_export;
	output		led8_export;
	input		msb_export;
	output	[7:0]	red_led_0_7_export;
	output		red_light_export;
	input		spi_MISO;
	output		spi_MOSI;
	output		spi_SCLK;
	output		spi_SS_n;
	output		spi_ss_export;
	input	[8:0]	sw0_8_export;
	input		uart_0_rxd;
	output		uart_0_txd;
	output		yellow_light_export;
endmodule
