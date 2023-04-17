#ifndef FILTERS_H_
#define FILTERS_H_
#include <systemc>
using namespace sc_core;

#include "tlm"
#include "tlm_utils/simple_target_socket.h"

#include "filter_def.h"

class DecimationFilter : public sc_module
{
public:
  tlm_utils::simple_target_socket<SobelFilter> t_skt;

  sc_fifo_in<float> i_x;
  sc_fifo_out<float> oy_result;

  SC_HAS_PROCESS(DecimationFilter);
  DecimationFilter(sc_module_name n);
  ~DecimationFilter() = default;

private:
  void do_filter();

  float x_row[3] = {0};

  float y_val;
  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};
#endif
