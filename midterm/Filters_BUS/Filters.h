#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#include "tlm"
#include "tlm_utils/simple_target_socket.h"

#include "filter_def.h"

class MeanFilter : public sc_module
{
public:
  tlm_utils::simple_target_socket<MeanFilter> t_skt;

  sc_fifo<double> i_r;

  sc_fifo<double> or_result;

  SC_HAS_PROCESS(MeanFilter);
  MeanFilter(sc_module_name n);
  ~MeanFilter() = default;

private:
  void do_filter();
  float X[128] = {0};

  float y_val;

  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};
#endif
