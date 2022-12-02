`define RAM_SIZE 4
// C ->h4, hE
//D -> h16
//E -> h7
//F ->h9
//G -> hA
//A -> hB
//B ->hD

//C#-> h1A
//D# -> h8

//F# ->h17
//G# -> h1C                  
//A# -> h18
                                    


module I2S (
	input CLK,
	input RESET,
	
	input Din,
	input LRCLK,
	input SCLK,
	output Dout,
	input [7:0] keycode,
	input [9:0] SW,
	
	input [1:0] ram_address,
	input ram_write,
	input [31:0] ram_writedata,
	output [31:0] ram_readdata
	
);

logic [23:0] ram [`RAM_SIZE];
logic [7:0] keycode_maps [7];

assign keycode_maps [0] = 8'h04;
assign keycode_maps [1] = 8'h16;
assign keycode_maps [2] = 8'h07;
assign keycode_maps [3] = 8'h09;
assign keycode_maps [4] = 8'h1A;
assign keycode_maps [5] = 8'h0B;
assign keycode_maps [6] = 8'h0D;


always_ff @ (posedge CLK or posedge RESET)
begin
	if (RESET)
	begin
		for (int i=0;i <`RAM_SIZE;i++)
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
logic [1:0] scale_counter [7];
logic [8:0] scale_counter_cap [7];

logic [23:0] sum;

assign sum = scale[0] + scale[1] + scale[2] + scale[3] + scale[4] + scale[5] + scale[6];

always_comb
begin
	scale_counter_cap[6] = 200/2;
	scale_counter_cap[5] = 179/2;
	scale_counter_cap[4] = 169/2;
	scale_counter_cap[3] = 150/2;
	scale_counter_cap[2] = 134/2;
	scale_counter_cap[1] = 126/2;
	scale_counter_cap[0] = 112/2;
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
		
			if (keycode == keycode_maps[i]) scale_counter_counter[i] <= scale_counter_counter[i] + 1;
			else 
			begin
				scale_counter_counter[i] <= 0;
				scale_counter[i] <= 0;
			end
			/*case(keycode)
				8'h04: begin
						scale_counter_counter[0] <= scale_counter_counter[0] + 1;
						end
				8'h16: begin
						scale_counter_counter[1] <= scale_counter_counter[1] + 1;
						end
				8'h07: begin
						scale_counter_counter[2] <= scale_counter_counter[2] + 1;
						end
				8'h09: begin
						scale_counter_counter[3] <= scale_counter_counter[3] + 1;
						end
				8'h1A: begin
						scale_counter_counter[4] <= scale_counter_counter[4] + 1;
						end
				8'h1B: begin
						scale_counter_counter[5] <= scale_counter_counter[5] + 1;
						end
				8'h1C: begin
						scale_counter_counter[6] <= scale_counter_counter[6] + 1;
						end
				default: begin
							scale_counter_counter[i] <= 0;
							scale_counter[i] <= 0;
							end
			endcase
			*/ //
			
//			if (SW[i]) scale_counter_counter[i] <= scale_counter_counter[i] + 1;
//			else
//			begin
//				scale_counter_counter[i] <= 0;
//				scale_counter[i] <= 0;
//			end
			
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

