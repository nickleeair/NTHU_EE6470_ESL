#include <cmath>
// include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif
#include "softmax.h"
#include "directives.h"

DecimationFilter::DecimationFilter(sc_module_name n)
    : sc_module(n)
{
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
// std::cout << mask[0] << " " << mask[1] << " " << mask[2] << std::endl;
#ifndef NATIVE_SYSTEMC
  i_data_1.clk_rst(i_clk, i_rst);
  i_data_2.clk_rst(i_clk, i_rst);
  o_result.clk_rst(i_clk, i_rst);
#endif
}

void DecimationFilter::do_filter()
{
  {
#ifndef NATIVE_SYSTEMC
    HLS_DEFINE_PROTOCOL("main_reset");
    i_data_1.reset();
    i_data_2.reset();
    o_result.reset();
#endif
    wait();
  }
  while (true)
  {
    result = 0;
    for (int j = 0; j < 16; j++)
    {
      LOOP_UNROLL;
      for (int i = 0; i < 8; i++)
      {
        LOOP_UNROLL;
#ifndef NATIVE_SYSTEMC
        {
          HLS_DEFINE_PROTOCOL("input");
          x[j][i] = i_data_1.get();
          y[j][i] = i_data_2.get();
          // wait();
        }
#else
        x[j][i] = i_data_1.read();
        y[j][i] = i_data_2.read();
#endif
      }
    }
    for (int i = 0; i < 16; i++)
    {
      LOOP_UNROLL;
      psum = 0;
      for (int j = 0; j < 16; j++)
      {
        LOOP_UNROLL;
        psum_matrix = 0;
        for (int k = 0; k < 8; k++)
        {
          LOOP_UNROLL;
          LOOP_PIPELINE;
          psum_matrix += x[i][k] * y[j][k];
        }
        // result = 1;
        buffer[16 * i + j] = 0;
        input_0_temp1 = 0;
        input_0_temp2 = 0;
        factorial = 1;
        // input_0 = i_data.read();
        // cout << "input_data =" << input_0 << endl;
        for (int k = 0; k < 36; k++)
        {
          LOOP_UNROLL;
          if (k == 0)
          {
            input_0_temp1 = psum_matrix;
          }
          else
          {
            input_0_temp1 = input_0_temp2 * psum_matrix;
          }
          factorial = factorial + k;
          input_0_temp2 = input_0_temp1 / factorial;
          buffer[16 * i + j] = buffer[16 * i + j] + input_0_temp2;
        }
        psum += buffer[16 * i + j];
      }
    }
    for (int i = 0; i < 256; i++)
    {
      LOOP_UNROLL;
      LOOP_PIPELINE;
      result = buffer[i] / psum;

#ifndef NATIVE_SYSTEMC
      {
        HLS_DEFINE_PROTOCOL("output");
        o_result.put(result);
      }
      // wait();
#else
      {
        for (int i = 0; i < 256; i++)
        {
          result = buffer[i] / psum;
          o_result.put(result);
        }
      }
#endif
    }
  }
}

/*for (i = 0; i < 8; i++)
{
  // get input from buffer and FIFO
  result = 1;
  input_0_temp1 = 0;
  input_0_temp2 = 0;
  factorial = 1;
  input_0 = i_data.read();
  cout << "input_data =" << input_0 << endl;
  for (int i = 0; i < 36; i++)
  {
    if (i == 0)
    {
      input_0_temp1 = input_0;
    }
    else
    {
      input_0_temp1 = input_0_temp2 * input_0;
    }
    cout << "multiplier result = " << input_0_temp1 << endl;
    factorial = factorial + i;
    input_0_temp2 = input_0_temp1 / factorial;
    cout << "accumulator result = " << input_0_temp2 << endl;
    psum = psum + input_0_temp2;
  }
  // result = result + 1;
  // cout << "result: " << result << endl;
  // o_result.write(result);
  //  save data to buffer
  buffer[i] = psum;
}
psum = buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4] + buffer[5] + buffer[6] + buffer[7];
// output
for (i = 0; i < 8; i++)
{
  result = 0;
  result = buffer[i] / psum;
  o_result.write(result);
}*/
