#ifndef SOBEL_FILTER1_H_
#define SOBEL_FILTER1_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

// const double mask[MASK_X][MASK_Y] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

struct SobelFilter1 : public sc_module
{
  tlm_utils::simple_target_socket<SobelFilter1> tsock;

  sc_fifo<unsigned char> i_data_1;
  sc_fifo<unsigned char> i_data_2;
  sc_fifo<double> o_result;

  SC_HAS_PROCESS(SobelFilter1);

  SobelFilter1(sc_module_name n) : sc_module(n),
                                   tsock("t_skt"),
                                   base_offset(0)
  {
    tsock.register_b_transport(this, &SobelFilter1::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~SobelFilter1()
  {
  }

  // int val[MASK_N];
  unsigned int base_offset;
  double psum_matrix;
  double buffer[256];
  double psum = 0;
  double x[16][8];
  double y[16][8];

  void do_filter()
  {
    {
      wait(CLOCK_PERIOD, SC_NS);
    }
    while (true)
    {
      for (int j = 0; j < 16; j++)
      {
        for (int i = 0; i < 8; i++)
        {

          x[j][i] = i_data_1.read();
          y[j][i] = i_data_2.read();
          cout << "input data:" << i << endl;
        }
      }
      for (int i = 0; i < 16; i++)
      {
        for (int j = 0; j < 16; j++)
        {

          for (int k = 0; k < 8; k++)
          {
            psum_matrix += x[i][k] * y[j][k];
          }
          // result = 1;
          double exp_result = 0;
          double input_0_temp1 = 0;
          double input_0_temp2 = 0;
          double factorial = 1;
          // input_0 = i_data.read();
          // cout << "input_data =" << input_0 << endl;
          for (int k = 0; k < 36; k++)
          {
            if (k == 0)
            {
              input_0_temp1 = psum_matrix;
            }
            else
            {
              input_0_temp1 = input_0_temp2 * psum_matrix;
            }
            // cout << "multiplier result = " << input_0_temp1 << endl;
            factorial = factorial + k;
            input_0_temp2 = input_0_temp1 / factorial;
            // cout << "accumulator result = " << input_0_temp2 << endl;
            exp_result = exp_result + input_0_temp2;
          }

          buffer[16 * i + j] = exp_result;
          // buffer[16 * i + j] = exp(psum_matrix);
          psum += buffer[16 * i + j];
        }
      }
      for (int i = 0; i < 256; i++)
      {
        double result = buffer[i] / psum;
        o_result.write(result);
      }
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay)
  {
    wait(delay);
    unsigned char *mask_ptr = payload.get_byte_enable_ptr(); // mask add
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

    switch (cmd)
    {
    case tlm::TLM_READ_COMMAND:
      cout << "READ" << endl;
      switch (addr)
      {
      case SOBEL_FILTER_RESULT_ADDR:
        buffer.uint = o_result.read();
        break;
      // case SOBEL_FILTER_CHECK_ADDR:
      //  buffer.uint = o_result.num_available();
      default:
        std::cerr << "READ Error! SobelFilter1::blocking_transport: address 0x"
                  << std::setfill('0') << std::setw(8) << std::hex << addr
                  << std::dec << " is not valid" << std::endl;
      }
      data_ptr[0] = buffer.uc[0];
      data_ptr[1] = 0;
      data_ptr[2] = 0;
      data_ptr[3] = 0;
      break;
    case tlm::TLM_WRITE_COMMAND:
      cout << "WRITE" << endl;
      switch (addr)
      {
      case SOBEL_FILTER_R_ADDR:
        i_data_1.write(data_ptr[0]);
        i_data_2.write(data_ptr[1]);
        break;
      default:
        std::cerr << "WRITE Error! SobelFilter1::blocking_transport: address 0x"
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
