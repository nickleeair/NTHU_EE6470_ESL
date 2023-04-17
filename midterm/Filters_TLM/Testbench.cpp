#include <cassert>
#include <cstdio>
#include <cstdlib>
using namespace std;
#include "filter_def.h"

#include "Testbench.h"

unsigned char header[54] = {
    0x42,        // identity : B
    0x4d,        // identity : M
    0, 0, 0, 0,  // file size
    0, 0,        // reserved1
    0, 0,        // reserved2
    54, 0, 0, 0, // RGB data offset
    40, 0, 0, 0, // struct BITMAPINFOHEADER size
    0, 0, 0, 0,  // bmp width
    0, 0, 0, 0,  // bmp height
    1, 0,        // planes
    24, 0,       // bit per pixel
    0, 0, 0, 0,  // compression
    0, 0, 0, 0,  // data size
    0, 0, 0, 0,  // h resolution
    0, 0, 0, 0,  // v resolution
    0, 0, 0, 0,  // used colors
    0, 0, 0, 0   // important colors
};

Testbench::Testbench(sc_module_name n)
    : sc_module(n), initiator("initiator")
{
  SC_THREAD(do_filter);
}

void Testbench::do_filter()
{
  int x, u;        // for loop counter
  unsigned char X; // color of R, G, B

  o_rst.write(false);
  o_rst.write(true);
  for (x = -1; x != 128; ++x)
  {
    if (x == -1)
    {
      X = 0;
    }
    else
    {
      X = x_input_signal[x];
    }
    o_x.write(X);
    // data_tran = data_tran + 1;
    wait(1); // emulate channel delay

    // std::cout<< "x =" << x <<endl;
    // std::cout<< "y =" << y <<endl;
    // std::cout<<"filter_result input"<<endl;
    if (x != 1)
      continue;
    else
    {
      if (iy_result.num_available() == 0)
        wait(iy_result.data_written_event());
      // std::cout<<"filter_result input"<<endl;
      // y_output[x] = iy_result.read();
      // std::cout<<"ir_filter_result input= "<<ir_result.read()<<endl;
      if (iy_result.read() == y_downsample_by2[x])
      {
        std::cout << "filter output= " << iy_result.read() << "is correct" << endl;
      }
      else
      {
        std::cout << "error GG" << endl;
      }
    }
  }
  // std::cout << "The total data transfer times (from tb to filter) = " << data_tran << endl;
  sc_stop();
}