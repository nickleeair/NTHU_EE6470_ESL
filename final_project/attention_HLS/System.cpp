#include "System.h"
System::System(sc_module_name n) : sc_module(n), tb("tb"), decimation_filter("decimation_filter"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	decimation_filter.i_clk(clk);
	decimation_filter.i_rst(rst);
	tb.o_data_1(i_data_1);
	tb.o_data_2(i_data_2);
	tb.i_result(o_result);
	decimation_filter.i_data_1(i_data_1);
	decimation_filter.i_data_2(i_data_2);
	decimation_filter.o_result(o_result);

	// tb.read_bmp(input_bmp);
}

System::~System()
{
}
