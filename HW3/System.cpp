#include "System.h"
System::System( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), filters("filters"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	filters.i_clk(clk);
	filters.i_rst(rst);
	tb.o_rgb(rgb);
	tb.ir_result(r_result);
	tb.ig_result(g_result);
	tb.ib_result(b_result);
	filters.i_rgb(rgb);
	filters.or_result(r_result);
	filters.og_result(g_result);
	filters.ob_result(b_result);

  tb.read_bmp(input_bmp);
}

System::~System() {
  tb.write_bmp(_output_bmp);
}
