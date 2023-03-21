#include <iostream>
#include <string>
using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "Filters.h"
#include "Testbench.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

// int main(int argc, char *argv[])
int sc_main(int argc, char **argv)
{
  if ((argc < 3) || (argc > 4))
  {
    cout << "No arguments for the executable : " << argv[0] << endl;
    cout << "Usage : >" << argv[0] << " in_image_file_name out_image_file_name"
         << endl;
    return 0;
  }

  // Create modules and signals
  Testbench tb("tb");
  MedianFilter median_filter("median_filter");
  MeanFilter mean_filter("mean_filter");
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
  sc_signal<bool> rst("rst");

  // Create FIFO channels
  sc_fifo<unsigned char> r;
  sc_fifo<unsigned char> g;
  sc_fifo<unsigned char> b;
  sc_fifo<double> r_result_1;
  sc_fifo<double> g_result_1;
  sc_fifo<double> b_result_1;
  sc_fifo<double> r_result;
  sc_fifo<double> g_result;
  sc_fifo<double> b_result;

  // Connect FIFO channels with modules
  tb.i_clk(clk);
  tb.o_rst(rst);
  median_filter.i_clk(clk);
  median_filter.i_rst(rst);
  mean_filter.i_clk(clk);
  mean_filter.i_rst(rst);
  tb.o_r(r);
  tb.o_g(g);
  tb.o_b(b);
  tb.ir_result(r_result);
  tb.ig_result(g_result);
  tb.ib_result(b_result);
  median_filter.i_r(r);
  median_filter.i_g(g);
  median_filter.i_b(b);
  median_filter.or_result(r_result_1);
  median_filter.og_result(g_result_1);
  median_filter.ob_result(b_result_1);
  mean_filter.i_r(r_result_1);
  mean_filter.i_g(g_result_1);
  mean_filter.i_b(b_result_1);
  mean_filter.or_result(r_result);
  mean_filter.og_result(g_result);
  mean_filter.ob_result(b_result);

  tb.read_bmp(argv[1]);
  sc_start();
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  tb.write_bmp(argv[2]);

  return 0;
}
