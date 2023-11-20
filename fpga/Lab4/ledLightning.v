module ledLightning(clk, Led);
input clk;
output [9:0] Led;
reg [9:0]Led;
reg direction;
initial
begin
	Led[9:1]<=9'b0;
	Led[0]<=1'b1;
	direction<=1'b1;
end

always@(posedge clk)
begin
	if(Led[1] && !direction)
		direction <= 1'b1;
	if(Led[8] && direction)
		direction <= 1'b0;
end

always@(posedge clk)
begin		
	if(direction)
	begin
		Led[0]<=Led[9];
		Led[1]<=Led[0];
		Led[2]<=Led[1];
		Led[3]<=Led[2];
		Led[4]<=Led[3];
		Led[5]<=Led[4];
		Led[6]<=Led[5];
		Led[7]<=Led[6];
		Led[8]<=Led[7];
		Led[9]<=Led[8];
	end
	else begin
		Led[8]<=Led[9];
		Led[7]<=Led[8];
		Led[6]<=Led[7];
		Led[5]<=Led[6];
		Led[4]<=Led[5];
		Led[3]<=Led[4];
		Led[2]<=Led[3];
		Led[1]<=Led[2];
		Led[0]<=Led[1];
		Led[9]<=Led[0];
	end
end
endmodule