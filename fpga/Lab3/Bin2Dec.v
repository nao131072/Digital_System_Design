module Bin2Dec(Sw, Seg7);
input [9:0] Sw;
output [31:0] Seg7;
wire [3:0] n0, n1, n2, n3;

assign n3 = Sw/1000;
assign n2 = (Sw%1000)/100;
assign n1 = (Sw%100)/10;
assign n0 = (Sw%10);

Seg7Decode s0(n0, Seg7[7:0]);
Seg7Decode s1(n1, Seg7[15:8]);
Seg7Decode s2(n2, Seg7[23:16]);
Seg7Decode s3(n3, Seg7[31:24]);
endmodule