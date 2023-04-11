#ifndef FILTERS_H_
#define FILTERS_H_
#include <systemc>
using namespace sc_core;

#include "tlm"
#include "tlm_utils/simple_target_socket.h"

#include "filter_def.h"

class Filters : public sc_module
{
public:
  tlm_utils::simple_target_socket<Filters> t_skt;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<double> or_result;
  sc_fifo<double> og_result;
  sc_fifo<double> ob_result;

  SC_HAS_PROCESS(Filters);
  Filters(sc_module_name n);
  ~Filters() = default;

private:
  void do_filter();
  char r_median_result[512][512] = {0};
  char g_median_result[512][512] = {0};
  char b_median_result[512][512] = {0};
  char r_sorted_pixels[9] = {0};
  char g_sorted_pixels[9] = {0};
  char b_sorted_pixels[9] = {0};

  double r_val;
  double g_val;
  double b_val;

  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};

#endif

