
module dual_processor (
	accel_I2C_SDAT,
	accel_I2C_SCLK,
	accel_G_SENSOR_CS_N,
	accel_G_SENSOR_INT,
	buzzer_external_connection_export,
	green_light_external_connection_export,
	hex012_external_connection_export,
	hex345_external_connection_export,
	key01_export,
	red_light_external_connection_export,
	sdram_addr,
	sdram_ba,
	sdram_cas_n,
	sdram_cke,
	sdram_cs_n,
	sdram_dq,
	sdram_dqm,
	sdram_ras_n,
	sdram_we_n,
	sdram_clk_clk,
	spi_MISO,
	spi_MOSI,
	spi_SCLK,
	spi_SS_n,
	spi_ss_external_connection_export,
	switch_export,
	sys_sdram_ref_clk_clk,
	sys_sdram_ref_reset_reset,
	us_counter_export,
	vga_CLK,
	vga_HS,
	vga_VS,
	vga_BLANK,
	vga_SYNC,
	vga_R,
	vga_G,
	vga_B,
	yellow_light_external_connection_export);	

	inout		accel_I2C_SDAT;
	output		accel_I2C_SCLK;
	output		accel_G_SENSOR_CS_N;
	input		accel_G_SENSOR_INT;
	output		buzzer_external_connection_export;
	output		green_light_external_connection_export;
	output	[23:0]	hex012_external_connection_export;
	output	[23:0]	hex345_external_connection_export;
	input	[1:0]	key01_export;
	output		red_light_external_connection_export;
	output	[12:0]	sdram_addr;
	output	[1:0]	sdram_ba;
	output		sdram_cas_n;
	output		sdram_cke;
	output		sdram_cs_n;
	inout	[15:0]	sdram_dq;
	output	[1:0]	sdram_dqm;
	output		sdram_ras_n;
	output		sdram_we_n;
	output		sdram_clk_clk;
	input		spi_MISO;
	output		spi_MOSI;
	output		spi_SCLK;
	output		spi_SS_n;
	output		spi_ss_external_connection_export;
	input	[9:0]	switch_export;
	input		sys_sdram_ref_clk_clk;
	input		sys_sdram_ref_reset_reset;
	input	[31:0]	us_counter_export;
	output		vga_CLK;
	output		vga_HS;
	output		vga_VS;
	output		vga_BLANK;
	output		vga_SYNC;
	output	[3:0]	vga_R;
	output	[3:0]	vga_G;
	output	[3:0]	vga_B;
	output		yellow_light_external_connection_export;
endmodule
