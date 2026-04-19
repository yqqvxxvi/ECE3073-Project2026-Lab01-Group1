/* Declare volatile pointers to I/O registers. This will ensure the
compiler
does not optimize the variable out of the disassembly*/
volatile int * Input_Key1 = (int *) 0x00003010; // check port address
volatile int * Output_LEDR2 = (int *) 0x00003020; // check port address
int main(void) {
int SW_value; // temporary place to store the Key1 value
while(1) {
SW_value=*(Input_Key1); // read Key1
*(Output_LEDR2) = SW_value; // transfer the value to LEDR 1
}
}


module latency_counter (
    input  wire        clk,            // CLOCK_50
    input  wire        leader_signal,   // Start trigger
    input  wire        follower_signal, // Stop trigger
    output reg [11:0]  latency_count    // 12-bit output
);

    reg counting; // Internal state to track if we are currently counting

    always @(posedge clk) begin
        if (leader_signal) begin
            // Start counting: reset count to 0 and set state to active
            latency_count <= 12'd0;
            counting <= 1'b1;
        end 
        else if (follower_signal) begin
            // Stop counting: keep the current value and set state to inactive
            counting <= 1'b0;
        end 
        else if (counting) begin
            // Increment only if we haven't reached the max 12-bit value (4095)
            if (latency_count < 12'hFFF) begin
                latency_count <= latency_count + 1'b1;
            end
        end
    end

endmodule

module top_level (
    input  wire        CLOCK_50,
    input  wire [1:0]  KEY,        // KEY[0] = Reset, KEY[1] = Input PIO
    output wire [9:0]  LEDR,       // LEDR[0] = Reset, LEDR[1] = Flag, LEDR[9:2] = Data
    inout  wire [35:0] GPIO,       // GPIO[2] = Leader, GPIO[10] = Follower
    output wire [6:0]  HEX0, HEX1, HEX2, HEX3, HEX4, HEX5 // 7-Segment Displays
);

    // --- 1. 20-bit Counter ---
    reg [19:0] counter_20bit;
    always @(posedge CLOCK_50) begin
        counter_20bit <= counter_20bit + 1'b1;
    end
    
    // Assign MSB to GPIO[2] (Leader Signal)
    assign GPIO[2] = counter_20bit[19];

    // --- 2. Latency Counter Instance ---
    wire [11:0] latency_val;
    latency_counter u0 (
        .clk             (CLOCK_50),
        .leader_signal   (GPIO[2]),     // Connected to MSB
        .follower_signal (GPIO[10]),    // Connected to Nios Flag PIO
        .latency_count   (latency_val)
    );

    // --- 3. Nios II System Instance ---
    // Make sure the port names match your Platform Designer (QSYS) generation
    nios_system u1 (
        .clk_clk                (CLOCK_50),
        .reset_reset_n          (KEY[0]),        // Active low reset
        .msb_pio_export         (counter_20bit[19]),
        .key1_pio_export        (KEY[1]),
        .flag_pio_export        (GPIO[10]),      // Output to GPIO and Counter
        .ledr_1_pio_export      (LEDR[1]),
        .ledr_9_2_pio_export    (LEDR[9:2])
    );

    // --- 4. Simple Assignments ---
    assign LEDR[0] = KEY[0]; // Visual confirmation of Reset

    // --- 5. HEX Display Driver ---
	 
    // You will need a BCD or Hex-to-7-Segment module here 
    // to convert latency_val into the 6 HEX outputs.
    // Example: hex_driver h0 (latency_val[3:0], HEX0); etc.
	

endmodule
