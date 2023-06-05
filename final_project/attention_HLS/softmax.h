#ifndef DECIMATION_FILTER_H_
#define DECIMATION_FILTER_H_
#include <systemc>
// #include <sysc/datatypes/fx/sc_ufixed.h>
//  #include "datatypes.h"
//  #include "ready_valid_port.h"
using namespace sc_core;
// using namespace sc_dt;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

// typedef float DT;

class DecimationFilter : public sc_module
{
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  // rdyvld_port_in<DT> i_data_1;
  // rdyvld_port_in<DT> i_data_2;
  // rdyvld_port_out<DT> o_result;

#ifndef NATIVE_SYSTEMC
  cynw_p2p<sc_dt::sc_uint<32>>::in i_data_1;
  cynw_p2p<sc_dt::sc_uint<32>>::in i_data_2;
  cynw_p2p<sc_dt::sc_uint<32>>::out o_result;
#else
  sc_fifo_in<sc_dt::sc_uint<32>>::in i_data_1;
  sc_fifo_in<sc_dt::sc_uint<32>>::in i_data_2;
  sc_fifo_out<sc_dt::sc_uint<32>>::out o_result;
#endif

  SC_HAS_PROCESS(DecimationFilter);
  DecimationFilter(sc_module_name n);
  ~DecimationFilter() = default;

private:
  int i;
  void do_filter();
  // float psum;
  // float buffer[8];

  // float input_0;
  sc_dt::sc_uint<32> result;
  sc_dt::sc_uint<32> x[16][8];
  sc_dt::sc_uint<32> y[16][8];
  // float pe[16][16];
  sc_dt::sc_uint<32> psum_matrix;

  sc_dt::sc_uint<32> input_0_temp1;
  sc_dt::sc_uint<32> input_0_temp2;
  int factorial;

  sc_dt::sc_uint<32> buffer[256];
  sc_dt::sc_uint<32> exp_result;
  sc_dt::sc_uint<32> psum;
};
#endif
