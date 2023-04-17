#include <cmath>

#include "Filters.h"

DecimationFilter::DecimationFilter(sc_module_name n) : sc_module(n)
{
    SC_THREAD(do_filter);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
}

const double mask[MASK_X] = {1 / 2, 1 / 3, 1 / 6};

void DecimationFilter::do_filter()
{
    
    for (int i = 0; i < 129; i++)
    {
        x_row[i] = 0;
        wait(1);
    }

    for (int i = 0; i < 129; i++)
    {
        if (i == 0)
        {
            x_row[i] = 0;
        }
        else
        {
            x_row[i] = i_x.read();
        }
        wait(1);
    }
    for (int i = 0; i < 129; i++)
    {
        y_val = (x_row[i + 2] * mask[0] + x_row[i + 1] * mask[1] + x_row[i] * mask[2]);
        // std::cout << "r_val_mean: " << r_val << std::endl;
        oy_result.write(y_val);
        // std::cout << "r_val_mean_input: " << r_val << std::endl;
        if(i==128){
            vld==1;
        }
        wait(1);
    }

    // emulate module delay
}
