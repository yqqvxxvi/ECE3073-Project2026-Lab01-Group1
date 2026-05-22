
module DualProcessorBase (
	pio_0_external_connection_export,
	shared_sdram_wire_addr,
	shared_sdram_wire_ba,
	shared_sdram_wire_cas_n,
	shared_sdram_wire_cke,
	shared_sdram_wire_cs_n,
	shared_sdram_wire_dq,
	shared_sdram_wire_dqm,
	shared_sdram_wire_ras_n,
	shared_sdram_wire_we_n,
	sys_sdram_pll_0_ref_clk_clk,
	sys_sdram_pll_0_ref_reset_reset);	

	input	[31:0]	pio_0_external_connection_export;
	output	[11:0]	shared_sdram_wire_addr;
	output	[1:0]	shared_sdram_wire_ba;
	output		shared_sdram_wire_cas_n;
	output		shared_sdram_wire_cke;
	output		shared_sdram_wire_cs_n;
	inout	[31:0]	shared_sdram_wire_dq;
	output	[3:0]	shared_sdram_wire_dqm;
	output		shared_sdram_wire_ras_n;
	output		shared_sdram_wire_we_n;
	input		sys_sdram_pll_0_ref_clk_clk;
	input		sys_sdram_pll_0_ref_reset_reset;
endmodule
