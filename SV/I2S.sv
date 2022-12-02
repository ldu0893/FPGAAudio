`define RAM_SIZE 8
`define LOG_SCALE 3

module I2S (
	input CLK,
	input RESET,
	
	input Din,
	input LRCLK,
	input SCLK,
	output Dout,
	
	input [9:0] SW,
	
	input [2:0] ram_address,
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

logic [23:0] scale [7];
logic [8:0] scale_counter_counter [7];
logic [`LOG_SCALE:0] scale_counter [7];
logic [8:0] scale_counter_cap [7];

logic [23:0] sum;

assign sum = scale[0] + scale[1] + scale[2] + scale[3] + scale[4] + scale[5] + scale[6];

always_comb
begin
// 1/(piano Hz) * 44.1kHz (sampling freq)
	scale_counter_cap[6] = 200 >> `LOG_SCALE; //A
	scale_counter_cap[5] = 179 >> `LOG_SCALE; //B
	scale_counter_cap[4] = 169 >> `LOG_SCALE; //C
	scale_counter_cap[3] = 150 >> `LOG_SCALE; //D
	scale_counter_cap[2] = 134 >> `LOG_SCALE; //E
	scale_counter_cap[1] = 126 >> `LOG_SCALE; //F
	scale_counter_cap[0] = 111 >> `LOG_SCALE; //G
end

always_comb
begin
	for (int i=0;i<7;i=i+1)
	begin
		scale[i] = ram[scale_counter[i]];
	end
end

always_ff @ (posedge LRCLK)
begin
	for (int i=0;i<7;i=i+1)
	begin
		if (scale_counter_counter[i] == scale_counter_cap[i])
		begin
			scale_counter[i] <= scale_counter[i] + 1;
			scale_counter_counter[i] <= 0;
		end
		else
		begin
			if (SW[i]) scale_counter_counter[i] <= scale_counter_counter[i] + 1;
			else
			begin
				scale_counter_counter[i] <= 0;
				scale_counter[i] <= 0;
			end
		end
//		if (SW[i]) scale_counter_counter[i] <= scale_counter_counter[i] + 1;
//		else scale_counter_counter[i] <= 0;
	end
end

//always_comb
//begin
//	for (int i=0;i<7;i++)
//	begin
//		if (scale_counter_counter[i] == scale_counter_cap[i])
//		begin
//			scale_counter[i] = scale_counter[i] + 1;
////			scale_counter_counter[i] = 0;
//		end
////		else
////		begin
////			scale_counter_counter[i] = scale_counter_counter[i] + 1;
////		end
//		scale[i] = ram[scale_counter[i]];
//	end
//end

//always_ff @ (posedge LRCLK)
//begin
//	if (counter_counter == SW)
//	begin
//		counter <= counter + 1;
//		counter_counter <= 0;
//	end
//	else
//	begin
//		counter_counter <= counter_counter + 1;
//	end
//end

always_ff @ (posedge SCLK)
begin
	if (!state && LRCLK)
	begin
		reg_din <= sum;
		reg_load <= 1;
		state <= 1;
	end
	else if (state && !LRCLK)
	begin
		reg_din <= sum;
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

