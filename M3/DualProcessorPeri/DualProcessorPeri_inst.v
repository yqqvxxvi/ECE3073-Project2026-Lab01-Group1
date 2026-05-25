	DualProcessorPeri u0 (
		.accel_I2C_SDAT                  (<connected-to-accel_I2C_SDAT>),                  //                     accel.I2C_SDAT
		.accel_I2C_SCLK                  (<connected-to-accel_I2C_SCLK>),                  //                          .I2C_SCLK
		.accel_G_SENSOR_CS_N             (<connected-to-accel_G_SENSOR_CS_N>),             //                          .G_SENSOR_CS_N
		.accel_G_SENSOR_INT              (<connected-to-accel_G_SENSOR_INT>),              //                          .G_SENSOR_INT
		.buzzer_export                   (<connected-to-buzzer_export>),                   //                    buzzer.export
		.hex012_export                   (<connected-to-hex012_export>),                   //                    hex012.export
		.hex345_export                   (<connected-to-hex345_export>),                   //                    hex345.export
		.image_address_export            (<connected-to-image_address_export>),            //             image_address.export
		.keys_export                     (<connected-to-keys_export>),                     //                      keys.export
		.led_green_export                (<connected-to-led_green_export>),                //                 led_green.export
		.led_red_export                  (<connected-to-led_red_export>),                  //                   led_red.export
		.led_yellow_export               (<connected-to-led_yellow_export>),               //                led_yellow.export
		.ledr_export                     (<connected-to-ledr_export>),                     //                      ledr.export
		.pixel_buffer_export             (<connected-to-pixel_buffer_export>),             //              pixel_buffer.export
		.shared_sdram_wire_addr          (<connected-to-shared_sdram_wire_addr>),          //         shared_sdram_wire.addr
		.shared_sdram_wire_ba            (<connected-to-shared_sdram_wire_ba>),            //                          .ba
		.shared_sdram_wire_cas_n         (<connected-to-shared_sdram_wire_cas_n>),         //                          .cas_n
		.shared_sdram_wire_cke           (<connected-to-shared_sdram_wire_cke>),           //                          .cke
		.shared_sdram_wire_cs_n          (<connected-to-shared_sdram_wire_cs_n>),          //                          .cs_n
		.shared_sdram_wire_dq            (<connected-to-shared_sdram_wire_dq>),            //                          .dq
		.shared_sdram_wire_dqm           (<connected-to-shared_sdram_wire_dqm>),           //                          .dqm
		.shared_sdram_wire_ras_n         (<connected-to-shared_sdram_wire_ras_n>),         //                          .ras_n
		.shared_sdram_wire_we_n          (<connected-to-shared_sdram_wire_we_n>),          //                          .we_n
		.spi_MISO                        (<connected-to-spi_MISO>),                        //                       spi.MISO
		.spi_MOSI                        (<connected-to-spi_MOSI>),                        //                          .MOSI
		.spi_SCLK                        (<connected-to-spi_SCLK>),                        //                          .SCLK
		.spi_SS_n                        (<connected-to-spi_SS_n>),                        //                          .SS_n
		.spi_ss_export                   (<connected-to-spi_ss_export>),                   //                    spi_ss.export
		.sw_export                       (<connected-to-sw_export>),                       //                        sw.export
		.sys_sdram_pll_0_ref_clk_clk     (<connected-to-sys_sdram_pll_0_ref_clk_clk>),     //   sys_sdram_pll_0_ref_clk.clk
		.sys_sdram_pll_0_ref_reset_reset (<connected-to-sys_sdram_pll_0_ref_reset_reset>), // sys_sdram_pll_0_ref_reset.reset
		.sys_sdram_pll_0_sdram_clk_clk   (<connected-to-sys_sdram_pll_0_sdram_clk_clk>),   // sys_sdram_pll_0_sdram_clk.clk
		.us_counter_export               (<connected-to-us_counter_export>),               //                us_counter.export
		.wern_export                     (<connected-to-wern_export>)                      //                      wern.export
	);

