#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "softmax_wrap.h"
#else
#include "softmax.h"
#endif

class System : public sc_module
{
public:
	SC_HAS_PROCESS(System);
	System(sc_module_name n);
	~System();

private:
	Testbench tb;
#ifndef NATIVE_SYSTEMC
	DecimationFilter_wrapper decimation_filter;
#else
	decimation_filter decimation_filter;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p<sc_dt::sc_uint<32>> i_data_1;
	cynw_p2p<sc_dt::sc_uint<32>> i_data_2;
	cynw_p2p<sc_dt::sc_uint<32>> o_result;
	// cynw_p2p< sc_dt::sc_uint<32> > b_result;
#else
	cynw_p2p<sc_dt::sc_uint<32>> i_data_1;
	cynw_p2p<sc_dt::sc_uint<32>> i_data_2;
	cynw_p2p<sc_dt::sc_uint<32>> o_result;
	// sc_fifo< sc_dt::sc_uint<32> > b_result;
#endif

	// std::string _output_bmp;
};
#endif
