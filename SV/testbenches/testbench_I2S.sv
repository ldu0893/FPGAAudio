//module testbench_I2S();
//
//timeunit 10ns;
//timeprecision 1ns;
//
//logic CLK, RESET, Din, LRCLK, SCLK, Dout, ram_address, ram_write;
//logic [31:0] ram_writedata, ram_readdata;
//
//logic [1:0] state_out;
//logic [31:0] channel_out;
//logic [4:0] counter_out;
//
//I2S_test u0(.*);
//
//always begin : CLOCK_GENERATION
//#1 CLK = ~CLK;
//end
//
//always begin : SCLK_GENERATION
//#2 SCLK = ~SCLK;
//end
//
//initial begin: CLOCK_INITIALIZATION
//    CLK = 0;
//end
//
//initial begin : SCLK_INIT
//	SCLK = 0;
//end
//
//initial begin: TEST_VECTORS
//RESET = 1;
//ram_address = 0;
//ram_write = 0;
//LRCLK = 0;
//ram_writedata = 31'h59d3a01f;
//
//#1 RESET = 0;
//
//#1 ram_write = 1;
//
//#2 ram_write = 0;
//
//#1 ram_writedata = 31'hf0f0f0f0;
//
//#1 ram_address = 1;
//
//#1 ram_write = 1;
//
//#2 ram_write = 0;
//
//#1 LRCLK = 1;
//
//#10 LRCLK = 0;
//
//#5 LRCLK = 1;
//
//#170 LRCLK = 0;
//
//#170 LRCLK = 1;
//
//
//
//end
//endmodule