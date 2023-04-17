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
  DecimationFilter decimation_filter("decimation_filter");
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
  sc_signal<bool> rst("rst");

  // Create FIFO channels
  sc_fifo<float> x;
  sc_fifo<float> y_result;

  // Connect FIFO channels with modules
  tb.i_clk(clk);
  tb.o_rst(rst);
  decimation_filter.i_clk(clk);
  decimation_filter.i_rst(rst);
  tb.o_x(x);
  tb.iy_result(y_result);
  decimation_filter.i_x(x);
  decimation_filter.oy_result(y_result);

  sc_start();
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  // tb.write_bmp(argv[2]);

  return 0;
}
