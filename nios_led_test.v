module nios_led_test (
    input  wire       CLOCK_50,
    input  wire [1:0] KEY,
    output wire [9:0] LEDR,

    input  wire       GSENSOR_SDO,
    output wire       GSENSOR_SDI,
    output wire       GSENSOR_SCLK,
    output wire       GSENSOR_CS_N
);

    nios_system u0 (
        .clk_clk          (CLOCK_50),
        .reset_reset_n    (KEY[0]),
        .led_export       (LEDR),

        .spi_accel_MISO   (GSENSOR_SDO),
        .spi_accel_MOSI   (GSENSOR_SDI),
        .spi_accel_SCLK   (GSENSOR_SCLK),
        .spi_accel_SS_n   (GSENSOR_CS_N)
    );

endmodule