`define RAM_SIZE 16
`define LOG_SCALE 10

`define NOTES 64

module I2S (
	input CLK,
	input RESET,
	
	input Din,
	input LRCLK,
	input SCLK,
	output Dout,
	
	input [9:0] SW,
	input [31:0] keycode,
	input [31:0] song,
	
	input [3:0] ram_address,
	input ram_write,
	input [31:0] ram_writedata,
	output [31:0] ram_readdata,
	
	input [5:0] timing_address,
	input timing_write,
	input [15:0] timing_writedata,
	output [15:0] timing_readdata
	
);

logic [23:0] ram [`RAM_SIZE];
logic [15:0] timing [64];

always_ff @ (posedge CLK or posedge RESET)
begin
	if (RESET)
	begin
		for (int i=0;i<64;i++) timing[i] <= 0;
	end
	else if (timing_write) timing[timing_address] <= timing_writedata;
end

assign timing_readdata = timing[timing_address];

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
		ram[ram_address] <= ram_writedata;//COULD THIS BE IT???
	end
end

assign ram_readdata = ram[ram_address];

logic reg_load;
logic [23:0] reg_din;

logic state;

lsreg reg0(.CLK(SCLK), .RESET, .load(reg_load), .Din(reg_din), .Dout(Dout));

logic [1:0] counter;
logic [9:0] counter_counter;

logic [23:0] scale [`NOTES];
logic [15:0] scale_counter_counter [`NOTES];
logic [`LOG_SCALE:0] scale_counter [`NOTES];
logic [15:0] scale_counter_cap [`NOTES];

logic [7:0] keycode_maps [`NOTES];
logic states [`NOTES];

always_comb
begin
	keycode_maps[4] = 8'h14;//C
	keycode_maps[3] = 8'h1a;//D
	keycode_maps[2] = 8'h08;//E
	keycode_maps[1] = 8'h15;//F
	keycode_maps[0] = 8'h17;//G
	keycode_maps[6] = 8'h1c;//A
	keycode_maps[5] = 8'h18;//B
	
	keycode_maps[7] = 8'h1f;//C#
	keycode_maps[8] = 8'h20;//D#
	keycode_maps[9] = 8'h22;//F#
	keycode_maps[10] = 8'h23;//G#
	keycode_maps[11] = 8'h24;//A#
	
	keycode_maps[23] = 8'h1d;//same config, lower reg
	keycode_maps[22] = 8'h1b;
	keycode_maps[21] = 8'h6;
	keycode_maps[20] = 8'h19;
	keycode_maps[19] = 8'h5;
	keycode_maps[18] = 8'h11;
	keycode_maps[17] = 8'h10;

	keycode_maps[16] = 8'h16;
	keycode_maps[15] = 8'h7;
	keycode_maps[14] = 8'ha;
	keycode_maps[13] = 8'hb;
	keycode_maps[12] = 8'hd;
	
	keycode_maps[35] = 8'h50;
	keycode_maps[34] = 8'h51;
	keycode_maps[33] = 8'h52;
	keycode_maps[32] = 8'h53;
	keycode_maps[31] = 8'h54;
	keycode_maps[30] = 8'h55;
	keycode_maps[29] = 8'h56;

	keycode_maps[28] = 8'h57;
	keycode_maps[27] = 8'h58;
	keycode_maps[26] = 8'h59;
	keycode_maps[25] = 8'h5a;
	keycode_maps[24] = 8'h5b;

	
	
	keycode_maps[47] = 8'h60;
	keycode_maps[46] = 8'h61;
	keycode_maps[45] = 8'h62;
	keycode_maps[44] = 8'h63;
	keycode_maps[43] = 8'h64;
	keycode_maps[42] = 8'h65;
	keycode_maps[41] = 8'h66;

	keycode_maps[40] = 8'h67;
	keycode_maps[39] = 8'h68;
	keycode_maps[38] = 8'h69;
	keycode_maps[37] = 8'h6a;
	keycode_maps[36] = 8'h6b;
	
	
	keycode_maps[59] = 8'h70;
	keycode_maps[58] = 8'h71;
	keycode_maps[57] = 8'h72;
	keycode_maps[56] = 8'h73;
	keycode_maps[55] = 8'h74;
	keycode_maps[54] = 8'h75;
	keycode_maps[53] = 8'h76;
	
	keycode_maps[52] = 8'h77;
	keycode_maps[51] = 8'h78;
	keycode_maps[50] = 8'h79;
	keycode_maps[49] = 8'h7a;
	keycode_maps[48] = 8'h7b;

end

logic [23:0] sum;

always_comb
begin
	sum=0;
	for (int i=0;i<`NOTES;i++) sum = sum + scale[i];
end

always_comb
begin
// 1/(piano Hz) * 44.1kHz (sampling freq)
//	scale_counter_cap[6] = 100 / 4; //A
//	scale_counter_cap[5] = 89 / 4; //B
//	scale_counter_cap[4] = 169 / 4; //C
//	scale_counter_cap[3] = 150 / 4; //D
//	scale_counter_cap[2] = 134 / 4; //E
//	scale_counter_cap[1] = 126 / 4; //F
//	scale_counter_cap[0] = 112 / 4; //G
//	
//	scale_counter_cap[7] = 159 / 4; //C#
//	scale_counter_cap[8] = 142 / 4; //D#
//	scale_counter_cap[9] = 119 / 4; //F#
//	scale_counter_cap[10] = 106 / 4; //G#
//	scale_counter_cap[11] = 95 / 4; //A#
	
	for (int i=0;i<`NOTES;i++) scale_counter_cap[i] = timing[i];
end

always_comb
begin
	for (int i=0;i<`NOTES;i=i+1)
	begin
		scale[i] = ram[scale_counter[i]];
	end
end

always_ff @ (posedge LRCLK)
begin
	for (int i=0;i<`NOTES;i=i+1)
	begin
		if (keycode[7:0] == keycode_maps[i] ||
			 keycode[15:8] == keycode_maps[i] ||
			 keycode[23:16] == keycode_maps[i] ||
			 keycode[31:24] == keycode_maps[i] ||
			 song[7:0] == keycode_maps[i] ||
			 song[15:8] == keycode_maps[i] ||
			 song[23:16] == keycode_maps[i] ||
			 song[31:24] == keycode_maps[i])
		begin
			states[i] <= 1;
		end
		else
		begin
			states[i] <= 0;
		end
	end
end

always_ff @ (posedge SCLK)
begin
	for (int i=0;i<`NOTES;i=i+1)
	begin
		if (scale_counter_counter[i] >= scale_counter_cap[i])
		begin
			scale_counter[i] <= scale_counter[i] + 1;
			scale_counter_counter[i] <= 0;
		end
		else
		begin
			if ((keycode[7:0] == keycode_maps[i] ||
				 keycode[15:8] == keycode_maps[i] ||
				 keycode[23:16] == keycode_maps[i] ||
				 keycode[31:24] == keycode_maps[i] ||
				 song[7:0] == keycode_maps[i] ||
				 song[15:8] == keycode_maps[i] ||
				 song[23:16] == keycode_maps[i] ||
				 song[31:24] == keycode_maps[i])
				 && states[i] == 1)
			begin
				scale_counter_counter[i] <= scale_counter_counter[i] + 1;
			end
			else
			begin
				if (states[i] == 0)
				begin
					scale_counter_counter[i] <= 0;
					scale_counter[i] <= 0;
				end
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

