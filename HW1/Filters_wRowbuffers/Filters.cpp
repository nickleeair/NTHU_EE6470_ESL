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
    int i, row, index;
    bool First_Time = 1;
    while (true)
    {
        // load r,g,b data in row buffer
         if (First_Time)
        {
            for (row = 0; row < 3; row++){
                for (i = 0; i < 514; i++){
                     std::cout << "first 3 AVAI: " << i_r.num_available() << std::endl;
                    r_row[0][i] = r_row[1][i];
                    g_row[0][i] = g_row[1][i];
                    b_row[0][i] = b_row[1][i];
                    r_row[1][i] = r_row[2][i];
                    g_row[1][i] = g_row[2][i];
                    b_row[1][i] = b_row[2][i];
                    r_row[2][i] = i_r.read();
                    g_row[2][i] = i_g.read();
                    b_row[2][i] = i_b.read();
                    wait(1);
                }
            }
        First_Time = 0;
        }
            else{
                for (i = 0; i < 514; i++){
                     std::cout << "not first 3 AVAI: " << i_r.num_available() << std::endl;
                    r_row[0][i] = r_row[1][i];
                    g_row[0][i] = g_row[1][i];
                    b_row[0][i] = b_row[1][i];
                    r_row[1][i] = r_row[2][i];
                    g_row[1][i] = g_row[2][i];
                    b_row[1][i] = b_row[2][i];
                    r_row[2][i] = i_r.read();
                    g_row[2][i] = i_g.read();
                    b_row[2][i] = i_b.read();
                }
                wait(258);
            }
              
        // load data from row buffer 
        for(int time=0; time<512; time++){
            for(i=0; i<3; i++){
               r_sorted_pixels[i] = r_row[0][i+time];
               r_sorted_pixels[i+3] = r_row[1][i+time];
               r_sorted_pixels[i+6] = r_row[2][i+time];
               g_sorted_pixels[i] = g_row[0][i+time];
               g_sorted_pixels[i+3] = g_row[1][i+time];
               g_sorted_pixels[i+6] = g_row[2][i+time];
               b_sorted_pixels[i] = b_row[0][i+time];
               b_sorted_pixels[i+3] = b_row[1][i+time];
               b_sorted_pixels[i+6] = b_row[2][i+time]; 
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
            std::cout << "r_val_median: " << r_val << std::endl;
            or_result.write(r_val);
            og_result.write(g_val);
            ob_result.write(b_val);
            std::cout << "r_val_median_input: " << r_val << std::endl;
            wait(1); // emulate module delay   
        }      
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

        std::cout << "r_val_mean_initial: " << r_val << std::endl;
        
        //loading input 
         if (First_time)
        {
            for (int row = 0; row < 3; row++){
                for (int i = 0; i < 512; i++){
                     std::cout << "first 3 AVAI: " << i_r.num_available() << std::endl;
                    r_row[0][i] = r_row[1][i];
                    g_row[0][i] = g_row[1][i];
                    b_row[0][i] = b_row[1][i];
                    r_row[1][i] = r_row[2][i];
                    g_row[1][i] = g_row[2][i];
                    b_row[1][i] = b_row[2][i];
                    r_row[2][i] = i_r.read();
                    g_row[2][i] = i_g.read();
                    b_row[2][i] = i_b.read();
                    wait(1);
                }
            }
        First_time = 0;
        }
            else{
                for (int i = 0; i < 512; i++){
                     std::cout << "not first 3 AVAI: " << i_r.num_available() << std::endl;
                    r_row[0][i] = r_row[1][i];
                    g_row[0][i] = g_row[1][i];
                    b_row[0][i] = b_row[1][i];
                    r_row[1][i] = r_row[2][i];
                    g_row[1][i] = g_row[2][i];
                    b_row[1][i] = b_row[2][i];
                    r_row[2][i] = i_r.read();
                    g_row[2][i] = i_g.read();
                    b_row[2][i] = i_b.read();
                }
                wait(258);
            }    

        for(int i=1; i<511;i++){
            r_val = (r_row[0][i-1] * mask[0][0] + r_row[0][i] * mask[0][1] + r_row[0][i+1] * mask[0][2] +
                     r_row[1][i-1] * mask[1][0] + r_row[1][i] * mask[1][1] + r_row[1][i+1] * mask[1][2] +
                     r_row[2][i-1] * mask[2][0] + r_row[2][i] * mask[2][1] + r_row[2][i+1] * mask[2][2])/10;
            g_val = (g_row[0][i-1] * mask[0][0] + g_row[0][i] * mask[0][1] + g_row[0][i+1] * mask[0][2] +
                     g_row[1][i-1] * mask[1][0] + g_row[1][i] * mask[1][1] + g_row[1][i+1] * mask[1][2] +
                     g_row[2][i-1] * mask[2][0] + g_row[2][i] * mask[2][1] + g_row[2][i+1] * mask[2][2])/10;
            b_val = (b_row[0][i-1] * mask[0][0] + b_row[0][i] * mask[0][1] + b_row[0][i+1] * mask[0][2] +
                     b_row[1][i-1] * mask[1][0] + b_row[1][i] * mask[1][1] + b_row[1][i+1] * mask[1][2] +
                     b_row[2][i-1] * mask[2][0] + b_row[2][i] * mask[2][1] + b_row[2][i+1] * mask[2][2])/10;
        std::cout << "r_val_mean: " << r_val << std::endl;
            or_result.write(r_val);
            std::cout << "r_val_mean_input: " << r_val << std::endl;
            
            og_result.write(g_val);
            //std::cout << "g_val_mean_input: " << r_val << std::endl;
            
            ob_result.write(b_val);
            //std::cout << "b_val_mean_input: " << r_val << std::endl;
            
            wait(1); // emulate module delay
            }  
        }
    }
