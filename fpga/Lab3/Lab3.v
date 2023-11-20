module Lab3(Sw, Seg7);
input [9:0] Sw;
output [31:0] Seg7;
Bin2Dec b2d_inst(Sw, Seg7);
endmodule
