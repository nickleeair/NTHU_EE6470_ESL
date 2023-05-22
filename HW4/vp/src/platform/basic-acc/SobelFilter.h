#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

const double mask[MASK_X][MASK_Y] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

struct SobelFilter : public sc_module {
  tlm_utils::simple_target_socket<SobelFilter> tsock;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<double> or_result;
  sc_fifo<double> og_result;
  sc_fifo<double> ob_result;

  SC_HAS_PROCESS(SobelFilter);

  SobelFilter(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &SobelFilter::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~SobelFilter() {
	}

  int val[MASK_N];
  unsigned int base_offset;

  void do_filter(){
    { wait(CLOCK_PERIOD, SC_NS); }
     while (true)
    {
        r_val = 0;
        g_val = 0;
        b_val = 0;

        // std::cout << "r_val_mean_initial: " << r_val << std::endl;
        for (unsigned int v = 0; v < MASK_Y; ++v)
        {
            for (unsigned int u = 0; u < MASK_X; ++u)
            {
                r_val += i_r.read() * mask[u][v];
                g_val += i_g.read() * mask[u][v];
                b_val += i_b.read() * mask[u][v];
            }
        }

        r_val = r_val / 10;
        g_val = g_val / 10;
        b_val = b_val / 10;

        // std::cout << "r_val_mean: " << r_val << std::endl;
        or_result.write(r_val);
        // std::cout << "r_val_mean_input: " << r_val << std::endl;

        og_result.write(g_val);
        // std::cout << "g_val_mean_input: " << g_val << std::endl;

        ob_result.write(b_val);
        // std::cout << "b_val_mean_input: " << b_val << std::endl;
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
     unsigned char *mask_ptr = payload.get_byte_enable_ptr(); //mask add
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case MEAN_FILTER_RESULT_ADDR:
            buffer1.uint = or_result.read();
            buffer2.uint = og_result.read();
            buffer3.uint = ob_result.read();
            break;
          case MEAN_FILTER_CHECK_ADDR:
            buffer1.uint = or_result.num_available();
            buffer2.uint = og_result.num_available();
            buffer3.uint = ob_result.num_available();
          default:
            std::cerr << "READ Error! SobelFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = 0;
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case MEAN_FILTER_R_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_r.write(data_ptr[0]);
      }
      if (mask_ptr[1] == 0xff) {
        i_g.write(data_ptr[1]);
      }
      if (mask_ptr[2] == 0xff) {
        i_b.write(data_ptr[2]);
      }
            break;
          default:
            std::cerr << "WRITE Error! SobelFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
        }
        break;
      case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
      }
      payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }
};
#endif
