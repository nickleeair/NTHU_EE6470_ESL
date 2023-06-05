#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;

#include <systemc>
#include <sysc/datatypes/fx/sc_ufixed.h>
// #include "datatypes.h"
// #include "ready_valid_port.h"
#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

using namespace sc_core;
// using namespace sc_dt;

// typedef float DT;

class Testbench : public sc_module
{
public:
  sc_in_clk i_clk;
  sc_out<bool> o_rst;
#ifndef NATIVE_SYSTEMC
  cynw_p2p<sc_dt::sc_uint<32>>::base_in i_result;
  cynw_p2p<sc_dt::sc_uint<32>>::base_out o_data_1;
  cynw_p2p<sc_dt::sc_uint<32>>::base_out o_data_2;
#else
  sc_fifo_out<sc_dt::sc_uint<32>> o_data_1;
  sc_fifo_out<sc_dt::sc_uint<32>> o_data_2;
  sc_fifo_in<sc_dt::sc_uint<32>> i_result;
#endif
  // rdyvld_port_out<DT> o_data_1;
  // rdyvld_port_out<DT> o_data_2;
  // rdyvld_port_in<DT> i_result;

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench() = default;

  sc_dt::sc_uint<32> y_output_signal[256];

private:
  int result_idx;
  void do_decimation_filter();
  // void sink();
  float result;
  float psum_matrix;

  float input_0_temp1;
  float input_0_temp2;
  float factorial;

  float buffer[256];
  float exp_result;
  float psum;
  float golden;
  sc_time total_start_time;
  sc_time total_run_time;
};
#endif