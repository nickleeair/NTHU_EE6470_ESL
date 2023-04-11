#include <iostream>
#include <string>
using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "Filters.h"
#include "Testbench.h"
//#include "Initiator.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

// int main(int argc, char *argv[])
int sc_main(int argc, char **argv) {

  if ((argc < 3) || (argc > 4)) {
    cout << "No arguments for the executable : " << argv[0] << endl;
    cout << "Usage : >" << argv[0] << " in_image_file_name out_image_file_name"
         << endl;
    return 0;
  }
  Testbench tb("tb");
  Filters Filters("filters");
  tb.initiator.i_skt(Filters.t_skt);

  tb.read_bmp(argv[1]);
  sc_start();
  //std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  tb.write_bmp(argv[2]);

  std::cout << (double)clock() / CLOCKS_PER_SEC << "S";

  return 0;
}
