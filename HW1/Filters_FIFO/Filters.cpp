#include <cmath>

#include "Filters.h"

MedianFilter::MedianFilter(sc_module_name n) : sc_module(n)
{
    SC_THREAD(do_filter);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
}

void MedianFilter::do_filter()
{
    int i;
    while (true)
    {
        // load r,g,b data in row buffer
            for(i=0; i<9; i++){
                r_sorted_pixels[i] = i_r.read();
                g_sorted_pixels[i] = i_g.read();
                b_sorted_pixels[i] = i_b.read();
                wait(1);
            }
            // sorting r, g, b
            for (int i = 0; i < 9; i++)
            {
                for (int j = i + 1; j < 9; j++)
                {
                    if (r_sorted_pixels[j] < r_sorted_pixels[i])
                    {
                        int temp_r = r_sorted_pixels[i];
                        r_sorted_pixels[i] = r_sorted_pixels[j];
                        r_sorted_pixels[j] = temp_r;
                    }
                    if (g_sorted_pixels[j] < g_sorted_pixels[i])
                    {
                        int temp_g = g_sorted_pixels[i];
                        g_sorted_pixels[i] = g_sorted_pixels[j];
                        g_sorted_pixels[j] = temp_g;
                    }
                    if (b_sorted_pixels[j] < b_sorted_pixels[i])
                    {
                        int temp_b = b_sorted_pixels[i];
                        b_sorted_pixels[i] = b_sorted_pixels[j];
                        b_sorted_pixels[j] = temp_b;
                    }
                    wait(1);
                }
            }
            r_val = r_sorted_pixels[4];
            g_val = g_sorted_pixels[4];
            b_val = b_sorted_pixels[4];
            //std::cout << "r_val_median: " << r_val << std::endl;
            or_result.write(r_val);
            og_result.write(g_val);
            ob_result.write(b_val);
            //std::cout << "r_val_median_input: " << r_val << std::endl;
            wait(1); // emulate module delay
        
    }
}

MeanFilter::MeanFilter(sc_module_name n) : sc_module(n)
{
    SC_THREAD(do_filter);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
}

const double mask[MASK_X][MASK_Y] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

void MeanFilter::do_filter()
{
    bool First_time = 1;
    while (true)
    {
        r_val = 0;
        g_val = 0;
        b_val = 0;

        //std::cout << "r_val_mean_initial: " << r_val << std::endl;
        
        //loading input 
        if(First_time){
            r_row[0] = 0;
            g_row[0] = 0;
            b_row[0] = 0;
            for(int i=1; i<9; i++){
                    r_row[i] = i_r.read();
                    g_row[i] = i_g.read();
                    b_row[i] = i_b.read();
                    wait(1);
                }
                First_time = 0;
        }
        else{
               for(int i=0; i<8; i++){
                        r_row[i] = r_row[i+1];
                        g_row[i] = g_row[i+1];
                        b_row[i] = b_row[i+1];
                        wait(1);
                    }
                r_row[8] = i_r.read();
                g_row[8] = i_g.read();
                b_row[8] = i_b.read();
                wait(1); 
            
                
        }    


            r_val = (r_row[0] * mask[0][0] + r_row[1] * mask[0][1] + r_row[2] * mask[0][2] +
                     r_row[3] * mask[1][0] + r_row[4] * mask[1][1] + r_row[5] * mask[1][2] +
                     r_row[6] * mask[2][0] + r_row[7] * mask[2][1] + r_row[8] * mask[2][2])/10;
            g_val = (g_row[0] * mask[0][0] + g_row[1] * mask[0][1] + g_row[2] * mask[0][2] +
                     g_row[3] * mask[1][0] + g_row[4] * mask[1][1] + g_row[5] * mask[1][2] +
                     g_row[6] * mask[2][0] + g_row[7] * mask[2][1] + g_row[8] * mask[2][2])/10;
            b_val = (b_row[0] * mask[0][0] + b_row[1] * mask[0][1] + b_row[2] * mask[0][2] +
                     b_row[3] * mask[1][0] + b_row[4] * mask[1][1] + b_row[5] * mask[1][2] +
                     b_row[6] * mask[2][0] + b_row[7] * mask[2][1] + b_row[8] * mask[2][2])/10;
          

            
            
            //std::cout << "r_val_mean: " << r_val << std::endl;
            or_result.write(r_val);
            //std::cout << "r_val_mean_input: " << r_val << std::endl;
            
            og_result.write(g_val);
            //std::cout << "g_val_mean_input: " << r_val << std::endl;
            
            ob_result.write(b_val);
            //std::cout << "b_val_mean_input: " << r_val << std::endl;
            
            wait(1); // emulate module delay
        }
    }
