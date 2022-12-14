//module I2S_test (
//	input CLK,
//	input RESET,
//
//	input Din,
//	input LRCLK,
//	input SCLK,
//	output Dout,
//
//	input ram_address,
//	input ram_write,
//	input [31:0] ram_writedata,
//	output [31:0] ram_readdata,
//
//	output [1:0] state_out,
//	output [31:0] channel_out,
//	output [4:0] counter_out
//
//);
//
//
//
//logic [31:0] ram [2];
//
//always_ff @ (posedge CLK or posedge RESET)
//begin
//	if (RESET)
//	begin
//		ram[0]<=0;
//		ram[1]<=0;
//	end
//	else if (ram_write)
//	begin
//		ram[ram_address] <= ram_writedata;
//	end
//end
//
//assign ram_readdata = ram[ram_address];
//
//logic [1:0] state = 1'b0;
//logic [31:0] channel;
//logic [4:0] counter;
//
//assign channel_out = channel;
//assign state_out = state;
//assign counter_out = counter;
//
//assign Dout = channel[31];
//
//always_ff @ (posedge SCLK)
//begin
//	if (state == 0 && LRCLK)
//	begin
//		counter <= 31;
//		state <= 1;
//		channel <= ram[0];
//	end
//	else if (state == 2 && !LRCLK)
//	begin
//		counter <= 31;
//		state <= 3;
//		channel <= ram[1];
//	end
//	if (counter != 0)
//	begin
//		counter <= counter - 1;
//		channel <= channel << 1;
//	end
//	else if (state == 1)
//		state <= 2;
//	else if (state == 3)
//		state <= 0;
//end
//
//endmodule