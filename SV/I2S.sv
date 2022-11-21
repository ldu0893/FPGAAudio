`define RAM_SIZE 4

module I2S (
	input CLK,
	input RESET,
	
	input Din,
	input LRCLK,
	input SCLK,
	output Dout,
	
	input [9:0] SW,
	
	input [1:0] ram_address,
	input ram_write,
	input [31:0] ram_writedata,
	output [31:0] ram_readdata
	
);

logic [23:0] ram [`RAM_SIZE];

always_ff @ (posedge CLK or posedge RESET)
begin
	if (RESET)
	begin
		for (int i=0;i<`RAM_SIZE;i++)
		begin
			ram[i] <= 0;
		end
	end
	else if (ram_write)
	begin
		ram[ram_address] <= ram_writedata[31:8];
	end
end

assign ram_readdata = ram[ram_address];

logic reg_load;
logic [23:0] reg_din;

logic state;

lsreg reg0(.CLK(SCLK), .RESET, .load(reg_load), .Din(reg_din), .Dout(Dout));

logic [1:0] counter;
logic [9:0] counter_counter;

always_ff @ (posedge LRCLK)
begin
	if (counter_counter == SW)
	begin
		counter <= counter + 1;
		counter_counter <= 0;
	end
	else
	begin
		counter_counter <= counter_counter + 1;
	end
end

always_ff @ (posedge SCLK)
begin
	if (!state && LRCLK)
	begin
		reg_din <= ram[counter];
		reg_load <= 1;
		state <= 1;
	end
	else if (state && !LRCLK)
	begin
		reg_din <= ram[counter];
		reg_load <= 1;
		state <= 0;
	end
	else
	begin
		reg_load <= 0;
	end
end

//logic [1:0] state = 1'b0;
//logic [31:0] channel;
//logic [4:0] counter;

//always_ff @ (posedge LRCLK)
//begin
//	channel <= ram[0];
//end
//
//always_ff @ (negedge LRCLK)
//begin
//	channel <= ram[1];
//end
//
//always_ff @ (posedge SCLK)
//begin
//	channel <= channel << 1;
//end
//
//assign Dout = channel[31];

//always_ff @ (posedge SCLK)
//begin
//	Dout <= Dout_async;
//end

//always_ff @ (posedge SCLK)
//begin
//	if (state == 0 && LRCLK)
//	begin
//		counter <= 23;
//		state <= 1;
//		channel <= ram[0];
//	end
//	else if (state == 2 && !LRCLK)
//	begin
//		counter <= 23;
//		state <= 3;
//		channel <= ram[1];
//	end
//	if (counter != 0)
//	begin
//		counter <= counter - 1;
//		channel <= channel << 1;
//	end
//	else if (state == 1)
//	begin
//		channel <= 0;
//		state <= 2;
//	end
//	else if (state == 3)
//	begin
//		channel <= 0;
//		state <= 0;
//	end
//end

endmodule

