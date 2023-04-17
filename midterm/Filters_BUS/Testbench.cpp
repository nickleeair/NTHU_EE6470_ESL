#include <cassert>
#include <cstdio>
#include <cstdlib>
using namespace std;

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
    : sc_module(n), initiator("initiator"), output_rgb_raw_data_offset(54)
{
  SC_THREAD(do_filter);
}

void Testbench::do_filter()
{
  int x, y, v, u; // for loop counter

  for (x = 0; x != 129; ++x)
  {
    if (x == 0)
    {
      X = 0;
    }
    else
    {
      X = x_input_signal[x - 1];
    }

    data.uc[0] = X;

    mask[0] = 0xff;
    mask[1] = 0;
    mask[2] = 0;
    mask[3] = 0;
    initiator.write_to_socket(MEAN_MM_BASE + MEAN_FILTER_R_ADDR, mask,
                              data.uc, 4);
    // wait(1 * CLOCK_PERIOD, SC_NS);

    // std::cout<<"x ="<<x<<std::endl;
    // std::cout<<"y ="<<y<<std::endl;
    initiator.read_from_socket(MEAN_MM_BASE + MEAN_FILTER_RESULT_ADDR, mask,
                               data.uc, 4);

    if (x < 3)
    {
      continue;
    }
    else
    {
      y_downsample_by2[x - 3] = data.uc[0];
    }

    //*(target_bitmap + bytes_per_pixel * (width * y + x) + 1) = data.uc[1];
    //*(target_bitmap + bytes_per_pixel * (width * y + x) + 0) = data.uc[2];
  }
  initiator.write_to_socket(RAM_MM_BASE, mask, header,
                            output_rgb_raw_data_offset);
  initiator.write_to_socket(RAM_MM_BASE + output_rgb_raw_data_offset, mask,
                            target_bitmap, bytes_per_pixel * height * width);
  sc_stop();
}
