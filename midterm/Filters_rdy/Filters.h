#ifndef FILTERS_H_
#define FILTERS_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class DecimationFilter : public sc_module
{
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<float> i_x;
  sc_fifo_out<float> oy_result;

  SC_HAS_PROCESS(DecimationFilter);
  DecimationFilter(sc_module_name n);
  ~DecimationFilter() = default;

private:
  void do_filter();

  float x_row[3] = {0};

  float y_val;
};
#endif
