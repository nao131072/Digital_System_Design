module Lab2(Sw, Led);
input [9:0] Sw;
output [9:0] Led;
	Switch2Led s21_inst(Sw, Led);
endmodule
