module my_pll(clk_in, clk_out);
input clk_in; //50MHz
output clk_out; //25Hz
reg clk_out;
parameter counter_max = 1_000_000;
reg [19:0] current;

initial
begin
	clk_out <= 1'b0;
	current <= counter_max;
end

always@(posedge clk_in)
begin
	if(current == 0)
	begin
		current <= counter_max;
		clk_out <= !clk_out;
	end
	else
		current <= current - 1;
end
endmodule