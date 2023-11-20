module Seg7Decode(num, seg7Decode);
input [3:0] num;
output reg [7:0] seg7Decode;
always@(num)
begin
	case(num)
		0 : seg7Decode = 8'b11000000; // 0
		1 : seg7Decode = 8'b11111001; // 1
		2 : seg7Decode = 8'b10100100; // 2
		3 : seg7Decode = 8'b10110000; // 3
		4 : seg7Decode = 8'b10011001; // 4
		5 : seg7Decode = 8'b10010010; // 5
		6 : seg7Decode = 8'b10000010; // 6
		7 : seg7Decode = 8'b11011000; // 7 
		8 : seg7Decode = 8'b10000000; // 8
		9 : seg7Decode = 8'b10010000; // 9
		default : 
			seg7Decode = 8'b11111111;
	endcase
end
endmodule