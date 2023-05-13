#ifndef FILTERS_H_
#define FILTERS_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class Filters : public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in<bool> i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p<sc_dt::sc_uint<24>>::in i_rgb;
	cynw_p2p<sc_dt::sc_uint<32>>::out or_result;
	cynw_p2p<sc_dt::sc_uint<32>>::out og_result;
	cynw_p2p<sc_dt::sc_uint<32>>::out ob_result;
#else
	sc_fifo_in<sc_dt::sc_uint<24>> i_rgb;
	sc_fifo_out<sc_dt::sc_uint<32>> or_result;
	sc_fifo_out<sc_dt::sc_uint<32>> og_result;
	sc_fifo_out<sc_dt::sc_uint<32>> ob_result;
#endif

	SC_HAS_PROCESS(Filters);
	Filters(sc_module_name n);
	~Filters();

private:
	void do_filter();
	int r_val;
	int g_val;
	int b_val;
};
#endif

