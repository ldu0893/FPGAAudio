module lsreg (
	input CLK, RESET, load,
	input [23:0] Din,
	output Dout
);

logic [23:0] data;

assign Dout = data[23];

always_ff @ (posedge CLK or posedge RESET)
begin
	if (RESET) data <= 0;
	else
	begin
		if (load) data <= Din;
		else
		begin
			data <= data << 1;
		end
	end
end

endmodule
