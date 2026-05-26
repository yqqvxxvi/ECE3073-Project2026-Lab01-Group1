	DualProcessorBase u0 (
		.pio_0_external_connection_export (<connected-to-pio_0_external_connection_export>), // pio_0_external_connection.export
		.shared_sdram_wire_addr           (<connected-to-shared_sdram_wire_addr>),           //         shared_sdram_wire.addr
		.shared_sdram_wire_ba             (<connected-to-shared_sdram_wire_ba>),             //                          .ba
		.shared_sdram_wire_cas_n          (<connected-to-shared_sdram_wire_cas_n>),          //                          .cas_n
		.shared_sdram_wire_cke            (<connected-to-shared_sdram_wire_cke>),            //                          .cke
		.shared_sdram_wire_cs_n           (<connected-to-shared_sdram_wire_cs_n>),           //                          .cs_n
		.shared_sdram_wire_dq             (<connected-to-shared_sdram_wire_dq>),             //                          .dq
		.shared_sdram_wire_dqm            (<connected-to-shared_sdram_wire_dqm>),            //                          .dqm
		.shared_sdram_wire_ras_n          (<connected-to-shared_sdram_wire_ras_n>),          //                          .ras_n
		.shared_sdram_wire_we_n           (<connected-to-shared_sdram_wire_we_n>),           //                          .we_n
		.sys_sdram_pll_0_ref_clk_clk      (<connected-to-sys_sdram_pll_0_ref_clk_clk>),      //   sys_sdram_pll_0_ref_clk.clk
		.sys_sdram_pll_0_ref_reset_reset  (<connected-to-sys_sdram_pll_0_ref_reset_reset>)   // sys_sdram_pll_0_ref_reset.reset
	);

